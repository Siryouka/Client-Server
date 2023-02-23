#ifndef DOMAIN_SOCKET_CLIENT_CC
#define DOMAIN_SOCKET_CLIENT_CC

#include "./domain_socket_client.h"


#include "../threading/threadload.h"

DomainSocketClient::~DomainSocketClient(){

	shm_unlink(this->shm_.shmpath_);

	
	delete this->buff_;
	this->buff_ = nullptr;
}


void *threadRoutine(void *arg){

	
	Payload *load = (Payload*)arg;

	size_t found;
	
	for(const auto &str:load->args_.search_section_){
		// search for str match
		//
		found = str.find(load->args_.target_);
		if(found != std::string::npos){
			// if match, add to hits
			//
			load->hits_.push_back(str);
		}
	}

	return nullptr;
}

// run the client and perform all needed tasks
void DomainSocketClient::runClient(const std::string &file_path, const std::string &search_phrase){
	// (1) open nameless Unix sock
	this->sock_ = socket(AF_UNIX,SOCK_STREAM,0);
	// check if sock error
	if(this->sock_ < 0){
		std::cout << "this->sock_ error\n";
		std::cerr << strerror(errno) << "\n";
		exit(-1);
	}

	// (2) connect to existing sock
	int success = connect(this->sock_,
		 				// sockaddr_un is unix sockaddr
						 reinterpret_cast<const sockaddr*>(&sock_addr_),
						 sizeof(sock_addr_)
						);
	// check if connect error
	if(success < 0){
		std::cout << "Domain Socket Client: success < 0 -> connection error\n";
		std::cerr << strerror(errno) << "\n";
		exit(-1);
	}
	// 1) Creates shared mem location and initializes

	this->shm_.shmpath_ = (char* )this->shared_mem_path_.data();
	this->shm_.fd_ = shm_open(this->shm_.shmpath_,O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);

	if(this->shm_.fd_ < 0){
		errExit("shm_open");
	}

	// set size of shared mem
	if(ftruncate(this->shm_.fd_,sizeof(struct shmbuf)) == -1){
		errExit("ftruncate");
	}

	// map obj to caller's addr space
	this->shm_.shmp_ = (shmbuf * )mmap(nullptr,sizeof(*this->shm_.shmp_),PROT_READ | PROT_WRITE, MAP_SHARED, this->shm_.fd_,0);

	if(this->shm_.shmp_ == MAP_FAILED){
		errExit("mmap");
	}

	// init the sems
	if(sem_init(&this->shm_.shmp_->sem1_,1,0) == -1){
		errExit("sem_init sem1");
	}
	if(sem_init(&this->shm_.shmp_->sem2_,1,0) == -1){
		errExit("sem_init sem1");
	}



	// 2) sends file path
	sendStr(this->sock_,file_path);

	// first check if file valid
	std::string file_validity_msg;
	readStr(this->sock_,file_validity_msg);

	if(file_validity_msg == this->MSG_INVALID_FILE_){
		std::cerr << this->MSG_INVALID_FILE_ << std::endl;
		// return nominative exit status
		exit(0);
	}

	// 3a) obtain the text content from shared mem
	std::string strTextContent;

	this->shm_.readStr(strTextContent);


	
	std::list<std::string> listStrContent {this->tr_.getSplitLines(strTextContent)};

	// quarter the file
	std::vector<std::list<std::string>> veclistStr;
	this->tr_.quarterList(listStrContent,veclistStr);

	// 3) threading
	const int kThreadCount {4};

	// create payloads
	Payload loads[kThreadCount];
	for(int i=0;i<kThreadCount;++i){
		Payload load;
		PayloadArgs pArgs;
		pArgs.setSearchSection(veclistStr[i]);
		pArgs.setTarget(search_phrase);
		load.args_ = pArgs;
		loads[i] = load;
	}

	pthread_t arrThreads[kThreadCount];
	for(int i=0;i<kThreadCount;++i){
		// create payload with the args
		//
		if(pthread_create(&arrThreads[i],nullptr,&threadRoutine,&loads[i]) != 0){
			perror("thread create");
			exit(1);
		}
	}

	// then wait for them to join back
	for(int i=0;i<kThreadCount;++i){
		if(pthread_join(arrThreads[i],nullptr) != 0){
			perror("bad join");
			exit(2);
		}
	}

	// print the hits
	int totalCount {1};
	for(int i=0;i<kThreadCount;++i){
		for(auto const &str:loads[i].hits_){
			std::cout << totalCount << "\t" << str << "\n";
			totalCount ++;
		}
	}

}


#endif