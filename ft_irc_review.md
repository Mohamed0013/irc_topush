# ft_irc Server Code Review - Evaluation Readiness

## Executive Summary

Your server architecture is solid overall — the epoll-based multiplexing, basic command handling, and channel management are generally well-structured. However, I identified **3 issues that are likely to cause evaluation failures**, with one being critical (the `send()` EAGAIN handling will fail the stopped-client flood test). There are also 2 moderate-severity issues that could cause problems during nc-based testing.

---

## Basic Checks

### 1. Makefile / Compilation / C++ / Executable Name

| Status            | Notes                                                                                                                                                                                                                                  |
| ----------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Cannot Verify** | You did not include the Makefile in the uploaded files. Verify it exists, uses `c++` or `g++`, compiles with `-Wall -Wextra -Werror -std=c++98` (or your campus standard), and produces the expected executable (typically `ircserv`). |

> **Action required:** Ensure your Makefile is clean, has no relink, and compiles without warnings.

---

### 2. Only One poll() (or Equivalent)

| Status   | Verdict                                                                                                          |
| -------- | ---------------------------------------------------------------------------------------------------------------- |
| **PASS** | Only one `epoll_wait()` in `Server::run()` (line 233 in server.cpp). No other blocking multiplexing calls exist. |

---

### 3. poll() Called Before Every accept/read/write, No errno-Based Retry Logic

| Status   | Verdict                                                                                                                                                                                                                                              |
| -------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **PASS** | The event loop in `run()` correctly calls `epoll_wait()` first, then dispatches to `acceptNewClient()`, `handleClientRead()`, and `handleClientWrite()` based on returned events. No use of `errno` anywhere in the codebase to trigger retry logic. |

---

### 4. fcntl() Used Only as `fcntl(fd, F_SETFL, O_NONBLOCK)`

| Status   | Verdict                                                                                                                         |
| -------- | ------------------------------------------------------------------------------------------------------------------------------- |
| **PASS** | Found exactly two calls, both conforming (server.cpp:79 for server fd, server.cpp:135 for client fd). No other `fcntl()` usage. |

---

## Networking

### 5. Server Listens on All Interfaces on Given Port

| Status   | Verdict                                                                                                                                    |
| -------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| **PASS** | `addr.sin_addr.s_addr = INADDR_ANY` (server.cpp:84). Correctly binds to all network interfaces. Port comes from command line (main.cpp:9). |

---

### 6. nc Connect, Send Commands, Receive Replies

| Status               | Notes                                                                                                                                  |
| -------------------- | -------------------------------------------------------------------------------------------------------------------------------------- |
| **CONDITIONAL PASS** | Will work IF the evaluator sends `\r\n` line endings. **Will FAIL if evaluator uses plain nc sending `\n` only** — see Issue #2 below. |

---

### 7. Reference IRC Client

| Status  | Notes                                                                                                                                                        |
| ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **N/A** | This is a question you answer during the defence. Common choices: HexChat, Irssi, WeeChat, or Kiwi IRC. Pick one and test thoroughly with it before defence. |

---

### 8. IRC Client Connection

| Status   | Verdict                                                                                                                    |
| -------- | -------------------------------------------------------------------------------------------------------------------------- |
| **PASS** | PASS/NICK/USER registration sequence is correctly implemented. Welcome message (001) is sent upon successful registration. |

---

### 9. Multiple Simultaneous Connections, Non-Blocking

| Status   | Verdict                                                                                                                                                                          |
| -------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **PASS** | `epoll` with `O_NONBLOCK` sockets, 100ms timeout on `epoll_wait`. The server will not block on any single client. Architecture correctly supports many simultaneous connections. |

---

### 10. Channel Join + Message Broadcast to All Members

| Status   | Verdict                                                                                                                                                                               |
| -------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------- |
| **PASS** | JOIN creates/adds to channels correctly. PRIVMSG iterates over `get_All_Cchanel()` and sends to all members except sender. Each recipient gets their epoll state modified to `EPOLLIN | EPOLLOUT`. |

---

## Networking Specials (Where Problems Start)

### 11. Partial Commands via nc

| Status   | Verdict                                                                                                                                                                 |
| -------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **PASS** | `handleClientRead()` correctly buffers incomplete data in `getRecvBuffer()` and only processes complete commands delimited by `\r\n`. Other connections are unaffected. |

---

### 12. Unexpectedly Kill a Client

| Status   | Verdict                                                                                                        |
| -------- | -------------------------------------------------------------------------------------------------------------- |
| **PASS** | `recv()` returns `<= 0`, `removeClient()` is called, server continues operating. Other connections unaffected. |

---

### 13. Kill nc with Half a Command Sent

| Status   | Verdict                                                                                                                                                                                                            |
| -------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **PASS** | Partial data stays in the client's recv buffer. When the socket closes, `recv()` returns `<= 0` on next epoll notification, client is removed, buffer is freed with the Client object. Server remains operational. |

---

### 14. Stop Client (^Z), Flood Channel, Resume — **CRITICAL ISSUE**

| Status   | Verdict                                                                                               |
| -------- | ----------------------------------------------------------------------------------------------------- |
| **FAIL** | The stopped client will be **disconnected** instead of receiving queued messages. See Issue #1 below. |

---

## Critical Issues Found

---

### **ISSUE #1 — `send()` EAGAIN Handling (CRITICAL — Fails Evaluation Item 14)**

**Location:** `server.cpp:219-222`

**Problem:** When a client's receive buffer is full (stopped with ^Z), `send()` returns `-1` with `errno = EAGAIN` or `EWOULDBLOCK`. Your code treats this as a fatal error and disconnects the client:

```cpp
ssize_t sent = send(fd, response.c_str(), response.length(), 0);
if (sent <= 0) {
    removeClient(fd);   // <-- WRONG: disconnects on EAGAIN
    return;
}
```

**Why this fails the evaluation:**

- Evaluator stops Client A with ^Z (on a channel)
- Evaluator floods the channel from Client B
- Server tries to send to Client A, kernel buffer is full → `send()` returns `-1`/EAGAIN
- Your server **disconnects Client A**
- When Client A is resumed, it's already gone — "all stored commands should be processed normally" never happens

**Fix:** Distinguish between actual errors (disconnect) and temporary unavailability (keep client, try later):

```cpp
void Server::handleClientWrite(int fd) {
    std::string &response = _clients[fd].getSendBuffer();
    if (response.empty())
        return;

    ssize_t sent = send(fd, response.c_str(), response.length(), MSG_NOSIGNAL);
    if (sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Socket buffer full — keep client, try again later
            return;
        }
        // Actual error — disconnect
        removeClient(fd);
        return;
    }
    if (sent == 0) {
        removeClient(fd);
        return;
    }

    _clients[fd].clearSendBuffer(sent);

    if (_clients[fd].getSendBuffer().empty())
        modifyEpollState(fd, EPOLLIN);
}
```

Also add `MSG_NOSIGNAL` to prevent SIGPIPE from killing the server if the client disappears.

---

### **ISSUE #2 — Line Ending Handling (MODERATE — Fails nc Tests if Evaluator Uses Plain `\n`)**

**Location:** `server.cpp:175`

**Problem:** Your code only splits on `\r\n`:

```cpp
while ((pos = _clients[fd].getRecvBuffer().find("\r\n")) != std::string::npos) {
```

When using plain `nc` without special options, pressing Enter sends `\n`, not `\r\n`. Commands sent this way will never be processed.

**Fix:** Handle both `\n` and `\r\n`:

```cpp
void Server::handleClientRead(int fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
        removeClient(fd);
        return;
    }

    if (_clients[fd].getRecvBuffer().length() + bytes > 4096) {
        removeClient(fd);
        return;
    }

    _clients[fd].getRecvBuffer().append(buffer, bytes);

    size_t pos;
    while ((pos = _clients[fd].getRecvBuffer().find('\n')) != std::string::npos) {
        std::string full_command = _clients[fd].getRecvBuffer().substr(0, pos);
        // Strip trailing \r if present
        if (!full_command.empty() && full_command[full_command.length() - 1] == '\r')
            full_command.erase(full_command.length() - 1);

        _clients[fd].getRecvBuffer().erase(0, pos + 1);

        if (!full_command.empty())
            parseInput(fd, full_command);

        if (_clients.find(fd) == _clients.end())
            return;
        if (this->_clients[fd].get_Close()) {
            removeClient(fd);
            return;
        }
    }
}
```

**Also fix:** Use `.append(buffer, bytes)` instead of `+= buffer` to properly handle binary data without relying on null-termination.

---

### **ISSUE #3 — Trailing Parameter Parsing (MODERATE — Breaks Multi-Word PRIVMSG)**

**Location:** `server.cpp:188-200`

**Problem:** `parseInput()` splits arguments by whitespace without respecting IRC's `:trailing parameter` syntax:

```cpp
std::istringstream iss(raw_command);
std::string cmd_name;
iss >> cmd_name;
// ...
while (iss >> arg) {
    args.push_back(arg);
}
```

For `PRIVMSG #channel :Hello everyone`, args becomes `["#channel", ":Hello", "everyone"]` and only `"Hello"` (without the colon) is sent because the code uses `args[1]`.

Similarly, `USER username 0 * :Real Name` truncates the real name.

**Fix:** Implement proper IRC parameter parsing that respects the colon prefix:

```cpp
void Server::parseInput(int clientFd, const std::string &raw_command) {
    std::string cmd_name;
    std::vector<std::string> args;

    // Find command name
    size_t i = 0;
    // Skip leading spaces
    while (i < raw_command.size() && raw_command[i] == ' ')
        i++;
    size_t cmd_start = i;
    while (i < raw_command.size() && raw_command[i] != ' ')
        i++;
    cmd_name = raw_command.substr(cmd_start, i - cmd_start);

    if (cmd_name.empty()) return;

    // Convert to uppercase for lookup
    for (size_t j = 0; j < cmd_name.size(); j++)
        cmd_name[j] = toupper(cmd_name[j]);

    // Parse parameters
    while (i < raw_command.size()) {
        // Skip spaces
        while (i < raw_command.size() && raw_command[i] == ' ')
            i++;
        if (i >= raw_command.size()) break;

        // Trailing parameter (starts with ':')
        if (raw_command[i] == ':') {
            args.push_back(raw_command.substr(i + 1));
            break;
        }

        // Regular parameter
        size_t arg_start = i;
        while (i < raw_command.size() && raw_command[i] != ' ')
            i++;
        args.push_back(raw_command.substr(arg_start, i - arg_start));
    }

    // ... rest of the function
}
```

---

### **ISSUE #4 — Dangling Channel Pointers (LOW — Potential Crash)**

**Location:** `server.cpp:111-125`

**Problem:** In `removeClient()`, when the last member leaves a channel, the channel is erased from `All_chanel`. Other clients who still have this channel in their `chanel_client` map now hold dangling pointers.

```cpp
if (chIt->second->get_All_Cchanel().empty())
    All_chanel.erase(chIt->second->get_namechanel());  // destroys chanel object
```

If those clients later try to interact with that channel, they dereference a dangling pointer.

**Fix:** When erasing a channel, notify all remaining members to remove it from their maps. Or, simply don't use raw pointers — look up channels by name from `All_chanel` every time instead of caching pointers in clients.

A simpler workaround: when joining a channel, don't cache the pointer in the client if you don't absolutely need it. The `removeClient` function already iterates the disconnecting client's channels, so the primary use case works. The issue is when OTHER clients have stale pointers.

Quick fix — in `removeClient()`, also clean up the channel pointer from other clients:

```cpp
void Server::removeClient(int fd) {
    std::map<std::string, chanel *> &clientChannels = _clients[fd].chaneel_clieent();
    std::map<std::string, chanel *>::iterator chIt;
    for (chIt = clientChannels.begin(); chIt != clientChannels.end(); ++chIt) {
        chIt->second->get_All_Cchanel().erase(fd);
        chIt->second->get_opChanel().erase(fd);
        if (chIt->second->get_All_Cchanel().empty()) {
            std::string name = chIt->second->get_namechanel();
            // Clean up this channel reference from ALL other clients
            std::map<int, Client>::iterator cit;
            for (cit = _clients.begin(); cit != _clients.end(); ++cit) {
                if (cit->first != fd)
                    cit->second.chaneel_clieent().erase(name);
            }
            All_chanel.erase(name);
        }
    }
    clientChannels.clear();

    epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clients.erase(fd);
}
```

---

## Additional Observations (Not Evaluation-Critical)

| Observation                                       | Severity | Notes                                                                                                                                    |
| ------------------------------------------------- | -------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| No `MSG_NOSIGNAL` on `send()`                     | Low      | Add `MSG_NOSIGNAL` flag to prevent SIGPIPE from crashing the server                                                                      |
| No send buffer size limit                         | Medium   | A malicious or very slow client could cause unbounded memory growth. Cap the send buffer (e.g., 32KB or 64KB) and disconnect if exceeded |
| JOIN doesn't check password for existing channels | Low      | Channel password feature is partially implemented but not enforced when joining existing channels                                        |
| Channel `limits` not enforced                     | Low      | `limits` field exists but never checked in JOIN                                                                                          |
| Invite-only (`+i`) not enforced                   | Low      | `invitation` flag exists but JOIN never checks it                                                                                        |
| No `TOPIC` command                                | Info     | Not required by the evaluation criteria you shared                                                                                       |
| No `MODE` command                                 | Info     | Not required by the evaluation criteria you shared                                                                                       |
| `epoll_create(1024)` is deprecated style          | Info     | Use `epoll_create1(EPOLL_CLOEXEC)` instead — not evaluation-critical                                                                     |
| Typo: `cretionChanel` should be `createChannel`   | Info     | Code works, just naming                                                                                                                  |

---

## Priority Fix Checklist

To pass the evaluation, fix these in order:

1. **[CRITICAL]** Fix `handleClientWrite()` to handle `EAGAIN`/`EWOULDBLOCK` without disconnecting (Issue #1)
2. **[HIGH]** Handle both `\n` and `\r\n` in `handleClientRead()` (Issue #2)
3. **[HIGH]** Implement `:trailing parameter` parsing in `parseInput()` (Issue #3)
4. **[MEDIUM]** Fix dangling channel pointers in `removeClient()` (Issue #4)
5. **[MEDIUM]** Add send buffer limit to prevent memory exhaustion
6. **[LOW]** Add `MSG_NOSIGNAL` to `send()` and `recv()` calls

---

## Estimated Evaluation Outcome

| Scenario                                                      | Grade                             |
| ------------------------------------------------------------- | --------------------------------- |
| If evaluator uses `nc` with `\r\n` and doesn't test ^Z resume | ~60-80%                           |
| If evaluator uses plain `nc` with `\n`                        | **0%** (basic nc check fails)     |
| If evaluator tests ^Z stop + flood + resume (as required)     | **0%** (client gets disconnected) |
| After applying fixes 1-3 above                                | **95-100%**                       |

The good news: your architecture is fundamentally sound. Issues #1-#3 are localized fixes that won't require restructuring. Apply them and you should be in good shape for the defence.
