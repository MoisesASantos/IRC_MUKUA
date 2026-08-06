*This project has been created as part of the 42 curriculum by sgaspar, mosantos, emjoao.*

# IRC_MUKUA

## Description

`IRC_MUKUA` is a 42 IRC server project written in C++98 for Linux. It uses non-blocking sockets and `epoll` to manage multiple clients in a single process.

The goal of the project is to provide a working IRC server where users can authenticate, join channels, exchange private and channel messages, and manage channel state such as operators, invitations, topics, passwords, and user limits.

## Features

- TCP IRC server with password-based authentication.
- Multiple clients handled concurrently with `epoll`.
- Non-blocking server and client sockets.
- Registration flow with `PASS`, `NICK`, and `USER`.
- Channel creation on `JOIN`.
- Channel operators and invite-only access.
- Channel password and user limit support.
- Topic management.
- Private messaging to users and channels.
- Basic moderation commands such as `KICK`, `INVITE`, and `MODE`.

## Instructions

- Linux environment.
- A C++ compiler with C++98 support.
- `make`.

### Compilation

```bash
make
```

This generates the `ircserv` executable.

### Cleaning

```bash
make clean
```

Removes object files.

```bash
make fclean
```

Removes object files and the executable.

```bash
make re
```

Rebuilds the project from scratch.

### Execution

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mysecret
```

### Arguments

- `port`: valid TCP port between `1` and `65535`.
- `password`: server password used by clients during the `PASS` command.

## Connection Flow

1. Start the server with a port and password.
2. Connect with an IRC client to the chosen port.
3. Send `PASS <password>`.
4. Send `NICK <nickname>`.
5. Send `USER <username> <hostname> <servername> :<realname>`.
6. Once authenticated and registered, you can join channels and use the supported commands.

## Supported Commands

### Authentication

- `PASS <password>`: authenticates the client against the server password.
- `NICK <nickname>`: sets the nickname.
- `USER <user> <host> <server> :<real name>`: sets the username and completes registration when paired with `NICK`.

### Channel and Messaging

- `JOIN <#channel> [key]`: creates the channel if needed and joins it.
- `PRIVMSG <target> :<message>`: sends a message to a user or a channel.
- `KICK <#channel> <nickname> [:reason]`: removes a user from a channel.
- `INVITE <nickname> <#channel>`: invites a user to a channel.
- `TOPIC <#channel> [:<new topic>]`: reads or updates the channel topic.
- `MODE <#channel> <mode> [argument]`: changes channel modes.
- `PING <token>`: replies with `PONG`.

## Channel Modes

The server currently supports these channel modes:

- `+i`: invite only.
- `+t`: topic protected, only operators can change it.
- `+k <key>`: channel password.
- `+l <limit>`: member limit.
- `+o <nickname>`: gives or removes operator status.

## Implementation Notes

- The server runs with `epoll` on Linux.
- Sockets are configured as non-blocking.
- Clients are stored by file descriptor and channels are stored by name.
- Channel membership and operator state are tracked inside the `Channel` class.
- Message parsing accepts IRC-style messages terminated by `\r\n`.

## Project Structure

- `src/main.cpp`: program entry point.
- `src/ft_engine.cpp`: main event loop and client dispatch.
- `src/Server/`: server lifecycle, socket setup, and client management.
- `src/Server/ServerHandler.cpp`: IRC command processing.
- `src/Client/`: client state and buffer handling.
- `src/channel/`: channel state, membership, and broadcast logic.
- `src/Headers/`: shared includes and declarations.

## Resources

### Classic References

- RFC 1459 - Internet Relay Chat Protocol.
- RFC 2812 - Internet Relay Chat: Client Protocol.
- cppreference.com - C++ language and standard library reference.
- Linux man pages for `socket`, `bind`, `listen`, `accept`, `recv`, `send`, `epoll`, and `fcntl`.

### AI Usage

AI was used to draft and structure this README, summarize the implemented IRC commands and channel modes from the source code, and refine the wording to match the 42 documentation requirements. It was not used to change the implementation of the server or to generate additional project features.