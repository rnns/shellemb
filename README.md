## Description

ShellEmb is a C command execution shell programmed for embedded devices(more specifically tested in routers) in a client/server scenario. The objective of this project is to work similarly to ncat, but saving space by being as light as possible to not occupy too much of embedded's memory. In here you will find the source code, binaries already compiled and stripped binaries for some architectures. If you wish to use it for an architecture that is not present in binaries folder, just grab the code and compile it with -static flag.

## Functionallity

The code works using sockets from C language to communicate between two points: client and server. The idea is to setup the server.c to be executed when device is booted and afterwards connect with the client to get access to command execution. Each command is executed on server with popen function.

## Usage

 The server has a binding port hard coded (default 7777) and the client requires the arguments [SERVER_IP_ADDRESS] and [PORT] to work properly. The server has a hard coded password (default "password") required to allow the command execution on shell. It is the first message you will need to send to perform authentication as the client. 
 It is possible to access and use the shell with netcat too. As long as the first message sent with netcat is the required password, the following commands will be executed and the output will be returned.
 Command execution errors outputed by the shell to stderr will not be returned to the client.
 Type help or h after authentication to receive a little help text.
 
## Example
```
./server
./client [SERVER_IP_ADDRESS] [PORT]
```
