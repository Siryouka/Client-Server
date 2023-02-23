#ifndef DOMAIN_SOCKET_CLIENT_H
#define DOMAIN_SOCKET_CLIENT_H

// include dom socket
#include "./unix_domain_socket.h"

class DomainSocketClient : public UnixDomainSocket{
	public:

		using ::UnixDomainSocket::UnixDomainSocket;
	
		~DomainSocketClient();

		void runClient(const std::string &file_path,const std::string &target_phrase);

};

#endif