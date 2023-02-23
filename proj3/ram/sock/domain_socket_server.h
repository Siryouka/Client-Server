#ifndef DOMAIN_SOCKET_SERVER_H
#define DOMAIN_SOCKET_SERVER_H


#include "./unix_domain_socket.h"

class DomainSocketServer: public UnixDomainSocket{
	public:
	 	
		using ::UnixDomainSocket::UnixDomainSocket;

		
		~DomainSocketServer();

		// main function to run server and listen for clients
		void runServer();

};

#endif