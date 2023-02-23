## (a) The files:

makefile

Text-server.cpp
Text-server.h

Text-client.cpp
Text-client.h

## (b) Their relationships:

Makefile is to complile text-server.cpp and text-client.cpp.
Header files contain function declarations, macro definitions and types used in source code.

## (c) The classes/functionality each file group provides


Class:DomainSocketClient: to establish a domain socket used both in server and client to connect.

Text-server.cpp:

int main: the main class to execute RunServer.
Void RunServer: the main function of:
1. Create a socket;
2. Bind socket to address for there server;
3. Listen for connections from clients;
4. Accept connection from a client;
5. Receive file name from client;
6. Receive search string from client;
7. Read the files;
8. Send quit signal to client.

Text-client.cpp:

int main: the main class to execute RunClient.
Void RunClient: the main function of:
1. Open the socket;
2. Connet to existing socket;
3. Write to socket;
4. Send file name to server, wait for server;
5. Send search string to server, wait for server;
6. Wait for server to send matched lines in the file;
7. Print out total bytes received.