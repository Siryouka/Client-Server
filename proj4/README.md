## lib
The directory "./lib" holds all of the .h and .cc files corresponding to the required classes. All of these classes are used to allow the program to work. 

## reader
It contains different useful classes.

- text_reader.h / .cc:
It stores all of the data and functions required to read and parse text files. It also does splitting lines. The class is found in UnixDomainSocket so that both the client and the server can parse text.

## shm
This folder contains classes of shared memory.

- shm.h / .cc
shm.h stores useful data such as a character buffer and semaphores. It is the basic unit that is put in shared memory for the client and server to share.
shm.cc is a wrapper between the client and the server. It contains useful methods to read and write from the shared memory.

## sock
This folder contains classes with regards to Unix Domain Sockets.

- unix_domain_socket.h / .cc:
This class is the parent class for both clients and server processes. The class stores the socket path, socket address, the buffer size / buffer, and some basic send / recieve functions for use by both the server and client.

- domain_socket_server.h / .cc:
This class stores all of the data and functions required for a server process. This class runs the server, waits for clients, reads client messages and map the file to shared memory.

- domain_socket_client.h / .cc:
This class stores all of the data and functions required for a client process. This class runs the client, sends data to the server, process the characters of the file-backed shared memory, replacing any  lower-case with upper-case ones.
## threading
This folder contains classes with regards to threads.

- threadload.h / .cc
This class represents a thread's payload. The payload contains the section of text that a thread needs to search through, along side with a target string to search. It also has a list that the thread writes to when any match is found.

##  client.cc
client.cc runs the client process. 
1. Checking if the number of arguments is valid;
2. Parses them for the path name
3. The DomainSocketClient class's runClient([FILE PATH] is called. 

## server.cc
server.cc runs the server process. 
1. The server calls runServer() to run the server and connect to shared memory that the client creates
2. The server then writes the contents of the text file to shared memory.

## makefile
makefile contains the commands to create the text-server and text-client files. These files are created by running make text-server and make text-client respectively. To remove these files, call make clean.

