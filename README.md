# ft_irc

A custom IRC (Internet Relay Chat) server implementation developed as part of the 42 School curriculum.

## Project Overview

`ft_irc` is a network programming project that recreates the behavior of an IRC server following the IRC protocol standards. The project focuses on socket programming, multiplexing, command parsing, client management, and server communication.

---

# Team Members & Responsibilities

### 👨‍💻 moel-yag

### Multiplexing

* Implemented multiplexing mechanisms for handling multiple client connections simultaneously.
* Managed socket monitoring and event-driven communication.
* Ensured efficient client/server interaction handling.

### 👨‍💻 alamiri

### Command Execution

* Implemented execution logic for IRC commands.
* Handled all mandatory IRC commands except bonus commands.
* Managed command responses and server-side behavior.

### 👨‍💻 mohdahma

### Parsing & Bonus Commands

* Developed the input parsing system (`parseInput`).
* Implemented and executed bonus commands:

  * `DCC`
  * `BOT`
* Managed command interpretation and validation.

---

# Features

* Multiple client support using multiplexing
* IRC protocol-inspired communication
* User authentication and nickname handling
* Channel creation and management
* Private messaging
* Command parsing and execution
* Bonus features:

  * File transfer using DCC
  * IRC Bot integration

---

# Architecture

The project uses **polymorphism** for command execution.

Each IRC command is implemented as a separate class inheriting from a common command interface/base class. Commands are dynamically dispatched through a command execution map:

```cpp
this->exCmd["PASS"] = new executePass();
this->exCmd["NICK"] = new executeNick();
this->exCmd["USER"] = new executeUser();
this->exCmd["PRIVMSG"] = new executePrivmsg();
this->exCmd["JOIN"] = new executeJoin();
this->exCmd["KICK"] = new executeKick();
this->exCmd["INVITE"] = new executeInvite();
this->exCmd["MODE"] = new executeMode();
this->exCmd["TOPIC"] = new executeTopic();
this->exCmd["DCC"] = new executeDcc();
this->exCmd["BOT"] = new executeBot();
```

This design improves:

* Extensibility
* Code organization
* Maintainability
* Separation of responsibilities

---

# IRC Commands

## Authentication Commands

### PASS

Authenticates the client with the server password.

```bash
PASS <password>
```

Example:

```bash
PASS mypassword
```

---

### NICK

Sets or changes the user's nickname.

```bash
NICK <nickname>
```

Example:

```bash
NICK mohamed
```

---

### USER

Registers the user connection.

```bash
USER <username> 0 * :<realname>
```

Example:

```bash
USER mohamed 0 * :Mohamed Ahma
```

---

# Messaging Commands

### PRIVMSG

Sends a private message to a user or channel.

```bash
PRIVMSG <target> :<message>
```

Examples:

```bash
PRIVMSG mohamed :Hello
```

```bash
PRIVMSG #general :Hello everyone
```

---

# Channel Commands

### JOIN

Joins or creates a channel.

```bash
JOIN <channel>
```

Example:

```bash
JOIN #42network
```

---

### KICK

Removes a user from a channel.

```bash
KICK <channel> <user> :<reason>
```

Example:

```bash
KICK #42network user1 :Spamming
```

---

### INVITE

Invites a user to a channel.

```bash
INVITE <nickname> <channel>
```

Example:

```bash
INVITE user1 #42network
```

---

### TOPIC

Sets or displays a channel topic.

```bash
TOPIC <channel> :<new topic>
```

Example:

```bash
TOPIC #42network :Welcome to ft_irc
```

---

### MODE

Manages channel modes and permissions.

```bash
MODE <channel> <modes> [parameters]
```

Examples:

```bash
MODE #42network +i
```

```bash
MODE #42network +k secretpass
```

```bash
MODE #42network +o user1
```

Supported modes:

* `+i` → Invite-only channel
* `+t` → Topic restricted to operators
* `+k` → Channel password
* `+o` → Give operator privileges
* `+l` → User limit

---

# Bonus Commands

## DCC

Direct Client-to-Client communication for file transfer.

```bash
DCC SEND <nickname> <filename>
```

Example:

```bash
DCC SEND user1 file.txt
```

---

## BOT

Interacts with the IRC bot.

```bash
BOT <message>
```

Example:

```bash
BOT hello
```

---

# Technologies Used

* C++
* Socket Programming
* TCP/IP Networking
* Poll Multiplexing
* Unix System Calls

---

# Build

```bash
make
```

# Run

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 password123
```

---

# Example Connection

Using `nc`:

```bash
nc localhost 6667
```

Then authenticate:

```bash
PASS password123
NICK user1
USER user1 0 * :User One
```

---

# Project Structure

```text
src/
├── parsing/
├── commands/
├── networking/
├── bonus/
└── utils/
```

---

# Notes

This project was developed collaboratively by 42 students as part of the networking and system programming curriculum.
