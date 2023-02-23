#ifndef UNIX_DOMAIN_SOCKET_H
#define UNIX_DOMAIN_SOCKET_H

#include <sys/socket.h>
#include <sys/un.h> 
#include <unistd.h>  

#include <cassert>  
#include <cerrno> 
#include <cstddef> 
#include <cstdlib>  
#include <cstring>  

#include <vector> 
#include <list> 

#include <string> 
#include <iostream> 
#include "../reader/text_reader.h"

#include "../shm/shm.h"

class UnixDomainSocket{
	public:
		explicit UnixDomainSocket();

	
		int sendInt(const int& sock_fd, const int &n);

		
		int readInt(const int& sock_fd, int &overwrite_int);

		void sendStr(const int &sock_fd,const std::string &msg);

		int readStr(const int &sock_fd,std::string &overwrite_str);

		
		void sendListStr(const int& sock_fd, const std::list<std::string> str_list);

		
		void readListStr(const int& sock_fd, std::list<std::string> &overwrite_list);


	protected:
		
		TextReader tr_;

		::sockaddr_un sock_addr_;	// sock addr from sys/un.h
		const std::string socket_path_ {"SOCKET_PATH"};	// let std::string manage char *

	
		const std::string shared_mem_path_ {"SHARED_MEM_PATH"};
		

		
		char * buff_;
	
		size_t buff_size_ {1024};
		
		int sock_;

		
		const std::string MSG_INVALID_FILE_ {"INVALID FILE"};


		const std::string MSG_VALID_FILE_ {"VALID FILE"};

	
		const char DELIM_ {'\0'};

		
		MyShm shm_;

};


#endif