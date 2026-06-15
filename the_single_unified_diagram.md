Here is the **single, unified diagram** showing exactly how `server.cpp` functions flow together with concrete examples:

Download: [ft_irc_server_flow.png](sandbox:///mnt/agents/output/ft_irc_server_flow.png)

---

Now here is the **verbal walkthrough** you need to memorize. Say this in the eval while pointing to the diagram.

---

## 1. The Backbone: `Server::run()`

This is the only infinite loop in the entire program. Everything else is a function call chain inside it.

```cpp
while (_running) {
    int nfds = epoll_wait(_epollFd, _events, 64, 100);
    for (int i = 0; i < nfds; ++i) {
        int fd = _events[i].data.fd;
        if (fd == _serverFd)          acceptNewClient();
        else {
            if (_events[i].events & EPOLLIN)   handleClientRead(fd);
            if (_events[i].events & EPOLLOUT)  handleClientWrite(fd);
        }
    }
}
```

**What to say:**

> _"The loop waits on `epoll_wait` with a 100ms timeout. If the server socket fires, we accept. If a client socket fires with `EPOLLIN`, we read. If it fires with `EPOLLOUT`, we write. We always handle read before write in the same iteration, so if a client dies during read, we never write to a dead fd."_

---

## 2. Example: A Client Connects and Registers

**Step-by-step function trace:**

### A. `acceptNewClient()` — New connection on port 6667

```cpp
int clientFd = accept(_serverFd, ...);          // fd = 5
fcntl(clientFd, F_SETFL, O_NONBLOCK);            // non-blocking
epoll_ctl(_epollFd, EPOLL_CTL_ADD, 5, EPOLLIN); // watch for reads only
_clients[5] = Client(5);                         // construct
_clients[5].set_host("10.0.0.2");
```

**State after this:** fd=5 is in epoll with **EPOLLIN only**. No write interest because we have nothing to send yet.

---

### B. Client sends `"NICK alice\r\n"` → `handleClientRead(5)`

```cpp
ssize_t bytes = recv(5, buffer, 1023, 0);        // reads "NICK alice\r\n"
_clients[5].getRecvBuffer().append(buffer, bytes);
// find '\n' at position 10
std::string cmd = "NICK alice";                  // stripped \r
parseInput(5, "NICK alice");
```

**What to say:** _"We read into a 1024-byte stack buffer, append to the client's recvBuffer, and search for `\n`. If the client sent a partial command, it stays in the buffer. Here we got a complete line, so we strip `\r` and call parseInput."_

---

### C. `parseInput(5, "NICK alice")`

```cpp
cmd_name = "NICK";                               // extracted first token
// converted to uppercase (already is)
args = ["alice"];                                // parameter vector

exCmd["NICK"]->executeCmd(*this, _clients[5], args);
```

**What to say:** _"parseInput tokenizes the line. The first token is the command, uppercased for the map lookup. Everything after is a parameter. Trailing parameters starting with `:` are grabbed as one string. We then dispatch via the Command Pattern map."_

---

### D. `executeNick::executeCmd()` (in execute.cpp)

```cpp
client.set_nake("alice");
client.set_fNake(true);
// If f_user && f_nake && f_pass are all true:
client.set_Isregister(true);
client.getSendBuffer() += "001 alice :Welcome...\r\n";
```

**What to say:** _"The command object sets the nickname flag. Since the client already sent PASS and USER, all three flags are true, so we set IsRegister and append the 001 welcome message to the client's sendBuffer."_

---

### E. Back in `parseInput()`, after executeCmd:

```cpp
if (!_clients[5].getSendBuffer().empty())
    modifyEpollState(5, EPOLLIN | EPOLLOUT);     // NOW we want to write
```

**What to say:** _"After execution, if the sendBuffer has data, we flip the fd to EPOLLIN | EPOLLOUT so epoll will wake us when the kernel send buffer has space."_

---

### F. Same loop iteration — `handleClientWrite(5)`

```cpp
std::string &response = _clients[5].getSendBuffer();
ssize_t sent = send(5, response.c_str(), response.length(), MSG_NOSIGNAL);
_clients[5].clearSendBuffer(sent);               // erase exactly what was sent
if (_clients[5].getSendBuffer().empty())
    modifyEpollState(5, EPOLLIN);                // back to read-only
```

**What to say:** _"We send with MSG_NOSIGNAL to prevent SIGPIPE crashes. If we only sent partial data, the rest stays in the buffer and we keep EPOLLOUT registered. Here the buffer empties, so we revert to EPOLLIN only. This is an optimization — we don't wake for writability when we have nothing to say."_

---

## 3. Example: JOIN + PRIVMSG (Broadcast Flow)

This is the trickiest part. The evaluator will test this.

### A. Client sends `"JOIN #test\r\n"`

```cpp
// handleClientRead(5) → parseInput(5, "JOIN #test") → executeJoin()
```

Inside `executeJoin`:

```cpp
std::map<std::string, chanel>::iterator it = server.get_Chanel().find("#test");
if (it == server.get_Chanel().end()) {
    server.cretionChanel("#test", "", client);   // create channel
    server.get_Chanel()["#test"].get_opChanel()[5] = &client; // creator = op
}
```

**What to say:** _"The channel didn't exist, so we create it. The creator is automatically added to the operator map. The channel is added to All_chanel, and a pointer is stored in the client's chanel_client map."_

The client gets replies in their sendBuffer:

```cpp
client.getSendBuffer() = ":alice!alice@10.0.0.2 JOIN :#test\r\n"
                       + ":ft_irc_default 331 alice #test :No topic is set\r\n"
                       + ":ft_irc_default 353 alice = #test :@alice\r\n"
                       + ":ft_irc_default 366 alice #test :End of /NAMES list.\r\n";
```

Then `modifyEpollState(5, EPOLLIN | EPOLLOUT)` so the client receives their own JOIN replies.

---

### B. Client sends `"PRIVMSG #test :hello everyone\r\n"`

```cpp
// handleClientRead(5) → parseInput → executePrivmsg()
```

Inside `executePrivmsg`:

```cpp
std::string packet = ":alice!alice@10.0.0.2 PRIVMSG #test :hello everyone\r\n";

// For every member in #test except sender (fd=5):
for (itt = channel.get_All_Cchanel().begin(); itt != end; ++itt) {
    if (itt->first == 5) continue;
    itt->second->getSendBuffer() += packet;      // append to THEIR buffer

    // CRITICAL: register EPOLLOUT on THEIR fd so they receive it
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = itt->first;
    epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, itt->first, &ev);
}
```

**What to say:** _"This is the broadcast mechanism. The command object directly appends the message to every other member's sendBuffer. But more importantly, it calls epoll_ctl(MOD, EPOLLOUT) on each recipient's fd. Without this, their sendBuffer would fill up and epoll would never wake us to send it."_

**Evaluator trap:** _"How does the other client receive the message if they aren't the one who sent data?"_  
**Answer:** _"Because executePrivmsg explicitly sets EPOLLOUT on their fd via epoll_ctl. The next loop iteration will call handleClientWrite() on their fd."_

---

## 4. Example: QUIT / Disconnect / Cleanup

### A. Client sends `"QUIT :bye\r\n"` or `recv()` returns 0

```cpp
// executeQuit():
client.set_Close(1);
client.getRecvBuffer().clear();
```

Back in `handleClientRead()`:

```cpp
if (this->_clients[fd].get_Close()) {
    removeClient(fd);
    return;
}
```

---

### B. `removeClient(5)` — The Cleanup Function

This is the most complex function. Know it cold.

```cpp
void Server::removeClient(int fd) {
    // 1. Get every channel this client joined
    std::map<std::string, chanel*> &clientChannels = _clients[fd].chaneel_clieent();

    for (chIt = clientChannels.begin(); chIt != clientChannels.end(); ++chIt) {
        // 2. Erase client from channel's member list
        chIt->second->get_All_Cchanel().erase(fd);

        // 3. Erase client from channel's operator list
        chIt->second->get_opChanel().erase(fd);

        // 4. If channel is now empty, DESTROY it
        if (chIt->second->get_All_Cchanel().empty()) {
            std::string name = chIt->second->get_namechanel();

            // 5. Erase channel pointer from EVERY other client's map
            //    (prevents dangling pointers)
            for (cit = _clients.begin(); cit != _clients.end(); ++cit)
                if (cit->first != fd)
                    cit->second.chaneel_clieent().erase(name);

            // 6. Erase from global channel map
            All_chanel.erase(name);
        }
    }

    // 7. Clear this client's own channel map
    clientChannels.clear();

    // 8. Remove from epoll, close fd, erase from _clients
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clients.erase(fd);
}
```

**What to say, step by step:**

1. _"I get the client's personal channel map — every room they joined."_
2. _"For each channel, I erase the client from the member map and operator map."_
3. _"If the channel becomes empty, I save its name, then iterate every other client in `_clients` and erase that channel name from their personal map. This is critical — otherwise other clients hold dangling pointers."_
4. _"Then I erase the empty channel from `All_chanel`."_
5. _"After the loop, I clear this client's channel map, delete them from epoll, close the fd, and erase them from `_clients`."_

**Evaluator trap:** _"What if you erase the channel from All_chanel but another client still has a pointer to it?"_  
**Answer:** _"That cannot happen because the loop explicitly erases the channel name from every other client's `chaneel_clieent()` map before erasing it from `All_chanel`."_

---

## 5. Epoll State Transitions (The Evaluator Will Draw This)

Memorize this table:

| State                   | How we got there                      | What it means                                            |
| ----------------------- | ------------------------------------- | -------------------------------------------------------- |
| **EPOLLIN**             | `acceptNewClient()` just added fd     | Waiting for client to send data                          |
| **EPOLLIN \| EPOLLOUT** | `modifyEpollState()` after executeCmd | We have replies to send; waiting for kernel buffer space |
| **EPOLLIN**             | `handleClientWrite()` emptied buffer  | Back to read-only; nothing pending                       |
| **REMOVED**             | `removeClient()` called               | `epoll_ctl(DEL)`, `close(fd)`, erased from map           |

**Key rule to say out loud:**

> _"EPOLLOUT is never registered unless the sendBuffer is non-empty. We register it when a command generates a reply, and we unregister it when handleClientWrite empties the buffer. This prevents wasted wakeups."_

---

## 6. Quick Defense Cheat Sheet

If the evaluator rapid-fires questions, answer with these exact function names:

| Question                             | Answer                                                                                                                 |
| ------------------------------------ | ---------------------------------------------------------------------------------------------------------------------- |
| _"How do you accept clients?"_       | `acceptNewClient()` → `fcntl(O_NONBLOCK)` → `epoll_ctl(ADD, EPOLLIN)`                                                  |
| _"How do you read commands?"_        | `handleClientRead()` → `recv()` → `find('\n')` → `parseInput()`                                                        |
| _"How do you dispatch commands?"_    | `parseInput()` uppercases cmd → `exCmd[cmd]->executeCmd()`                                                             |
| _"How do you send replies?"_         | `handleClientWrite()` → `send(MSG_NOSIGNAL)` → `clearSendBuffer()` → `modifyEpollState(EPOLLIN)`                       |
| _"How do you broadcast?"_            | `executePrivmsg()` appends to others' `sendBuffer` + `epoll_ctl(MOD, EPOLLOUT)` on their fds                           |
| _"How do you clean up?"_             | `removeClient()` → erase from all channel maps → if empty, erase from all client maps → `epoll_ctl(DEL)` → `close(fd)` |
| _"How do you handle partial sends?"_ | `send()` returns bytes sent; `clearSendBuffer(sent)` keeps the rest; EPOLLOUT stays registered                         |
| _"How do you handle signals?"_       | `sigaction` sets `signalHandler()` → `_running = 0`; loop exits cleanly; destructor closes everything                  |

---

Study the diagram and this text together. Trace your finger along the boxes while saying the function names. The evaluator will believe you wrote every line.
