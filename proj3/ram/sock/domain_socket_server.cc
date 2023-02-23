#ifndef DOMAIN_SOCKET_SERVER_CC
#define DOMAIN_SOCKET_SERVER_CC


#include <sys/sysinfo.h>
#include "./domain_socket_server.h"
#include <list>

DomainSocketServer::~DomainSocketServer(){

	delete this->buff_;
	this->buff_ = nullptr;
}

void DomainSocketServer::runServer(){
	this->sock_; // unnamed sock fd
	int client_req_sock_fd; // client connect request socket fd
	
	// 1) create a socket
	// 		AF_UNIX -> file system pathnames
	// 		SOCK_STREAM -> sequenced bytestream
	// 		0 -> default protocol (let OS decide correct protocol)
	this->sock_= ::socket(AF_UNIX,SOCK_STREAM,0);

	// see if error on creation
	//
	if(this->sock_ < 0){
		std::cerr << ::strerror(errno) << std::endl;
		::exit(-1);
	}

	// 2) bind sock to addr for server
	//
	unlink(socket_path_.c_str()); // sys call to delete file if it exists
								  // already using Unix sys calls for 
								  // sockets
	int success = ::bind(this->sock_,
						// sockaddr_un is unix sockaddr and so may be cast "up"
						// to pointer type ("C" polymorphism)
						reinterpret_cast<const sockaddr*>(&sock_addr_),
						// size needs to be known due to underlying data layout,
						// 		ie, could be size difference between parent and child
						sizeof(sock_addr_)); 
	// check if success
	//
	if(success < 0){
		std::cerr << strerror(errno) << std::endl;
		exit(-1);
	}

	// (3) listen to connections from clients
	// this value depends on number of cores, for right now hard coded


	
	size_t kMax_client_conns = get_nprocs_conf() - 1;
	success = listen(this->sock_, kMax_client_conns);

	// check if listen failure
	//
	if(success < 0){
		std::cerr << strerror(errno) << std::endl;
		exit(-1);
	}


	
	//  1)
	std::cout<< "SERVER STARTED" << std::endl;

	while(true){
		// (4) accept conn from client
		//
		client_req_sock_fd = accept(this->sock_,nullptr,nullptr);
		// check if conn failure
		//
		if(client_req_sock_fd < 0){
			std::cerr << strerror(errno) << "\n";
			// continue even if client fails to conn
			//
			continue;
		}

		
		// recieve file path and target

		// first recieve path
		std::string file_path {};

		// read file path
		readStr(client_req_sock_fd,file_path);

		// print to screen
		// 2) writes client request recieved
		std::clog << "CLIENT REQUEST RECEIVED\n";

		// opens shared mem
		this->shm_.shmpath_ = (char* )this->shared_mem_path_.data();
		this->shm_.fd_ = shm_open(this->shm_.shmpath_,O_RDWR,0);

		if(this->shm_.fd_ < 0){
			errExit("shm_open");
		}

		// attach to shared mem
		this->shm_.shmp_ = (shmbuf*) mmap(nullptr,sizeof(*this->shm_.shmp_),
								PROT_READ | PROT_WRITE, MAP_SHARED,
								this->shm_.fd_,0);
		// check if worked
		//
		if(this->shm_.shmp_ == MAP_FAILED){
			errExit("mmap");
		}
		// 3) Opens the shared memory
		std::clog << "\tMEMORY OPEN\n";

		// 4) opens and reads the file
		std::clog << "\tOPENING: " << file_path << "\n";

		// read text file
		//
		if(this->tr_.readTextFile(file_path)){
			// file path works
			// send valid file msg
			//
			this->sendStr(client_req_sock_fd,this->MSG_VALID_FILE_);

			// write to shared mem
			//
			this->shm_.writeStr(this->tr_.getTextContent());

			// 4) 
			std::clog << "\tFILE CLOSED\n";

			int result = ::munmap(this->shm_.shmpath_,sizeof(this->shm_.shmpath_));
			std::clog << "\tMEMORY CLOSED\n";

		}else{
			// file path invalid
			this->sendStr(client_req_sock_fd,this->MSG_INVALID_FILE_);

			// send bad file name
			// kill the client
			close(client_req_sock_fd);
		}
	}
}


#endif