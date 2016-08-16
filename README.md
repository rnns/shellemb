# ShellEmb

An interactive C command execution shell programmed for embedded devices(more specifically tested in routers) in a client/server scenario. The objective of this project is to work similarly to a ncat tool, but saving space by being as light as possible to not occupy too much of embedded's memory. In here you will find the source code, binaries already compiled and stripped binaries for some architectures. If you wish to use it for a architecture that is not present in binaries, just grab the code and compile it with -static flag.

# Functionallity

The code works using sockets from C language to communicate between two points: client and server. The idea is to setup the server.c to be executed when device is booted and afterwards connect with the client to get access to command execution. Each command is executed on server with popen function.

# Usage

 The server has a binding port hard coded (default 7777) and the client requires the arguments [TARGET_HOST_IP_ADDRESS] and [PORT] to work properly. The server has a hard coded password as well (default "password"), and it is the first message you will need to send to perform authentication when the client connects.
 
 # Usage Example
 
 ./server
 ./client [TARGET_HOST_IP_ADDRESS] [PORT]
 
