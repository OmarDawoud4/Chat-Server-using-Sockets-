# Chat Server  using Sockets in C

simple chat server and client application using the `poll` system call (Posix API)for handling multiple input sources asynchronously (stdin and socket). It is implemented in C using sockets.

## Files

- `server.c`:code for the chat server , which listens for incoming connections and communicates with the client.
- `client.c`: code for the chat  client , which connects to the server and allows for sending and receiving messages.

## Requirements 

 you need:
-  C compiler (gcc)
- Linux/Unix environment ( to be able to use Posix )

## How it Works

- **Server**: 
  - The server listens for connections on port `8080`(you can change it ) using the TCP protocol.
  - Once a client connects, it can receive messages from the client and send messages back to the client.
  - The `poll` system call is used to wait for input from either the stdin (to send messages) or the client socket (to receive messages).

- **Client**:
  - The client connects to the server using the IP address `127.0.0.1` (localhost) on port `8080`.
  - Similar to the server, it uses `poll` to handle inputs from both stdin (to send messages) and the server (to receive messages).

## Compilation

To compile both the server and client, run:

```bash
gcc -o server server.c
gcc -o client client.c
