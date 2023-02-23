#ifndef DOMAIN_SOCKET_CLIENT_CC
#define DOMAIN_SOCKET_CLIENT_CC

#include "./domain_socket_client.h"


#include "../threading/threadload.h"
#include <sstream>

DomainSocketClient::~DomainSocketClient(){

	shm_unlink(this->shm_.shmpath_);

	
	delete this->buff_;
	this->buff_ = nullptr;
}


void *threadRoutine(void *arg){

	std::vector<std::string> *search_section = (std::vector<std::string> *) arg;

	
	for(auto str = std::begin(*search_section); str !=std::end(*search_section); str++){
		// search for str match
		//
		for (int i = 0; i < str->size(); i++) {
			if (islower(str->at(i)))
			{
				char mm = str->at(i);
				str->replace(i,1,std::string(1,toupper(mm)));
				//*str += "\n";
			}
			//load->hits_.push_back(str);
		}
		/*found = str.find(load->args_.target_);
		if(found != std::string::npos){
			// if match, add to hits
			//
			load->hits_.push_back(str);
		}*/
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

	//this->shm_.shmpath_ = (char* )this->shared_mem_path_.data();
	/*this->shm_.fd_ = shm_open(this->shm_.shmpath_,O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);

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
	}*/

	// init the sems
	
	/*if(sem_init(&this->shm_.shmp_->sem1_,1,0) == -1){
		errExit("sem_init sem1");
	}
	if(sem_init(&this->shm_.shmp_->sem2_,1,0) == -1){
		errExit("sem_init sem1");
	}*/


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

	//this->shm_.readStr(strTextContent);

	//read from file shared memory
	char *addr;
	int fd;

	struct stat sv;

	fd = open(file_path.c_str(), O_RDWR);

	stat(file_path.c_str(), &sv);

	addr = static_cast<char *> (mmap(NULL, sv.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

	strTextContent = std::string(addr);


	std::vector<std::string> vector;

	
    std::string lines;
    std::stringstream ss(strTextContent);
    while(std::getline(ss, lines, '\n')) {
        vector.push_back(lines);
    }

		
	int line = vector.size()/4;
	
	
	std::vector<std::string> vector1;
	std::vector<std::string> vector2;
	std::vector<std::string> vector3;
	std::vector<std::string> vector4;
	for (int i = 0; i < line; i++) {
		vector1.push_back(vector.at(i));
	}
	
	for(int i = line; i < line*2; i++) {
		vector2.push_back(vector.at(i));
	}
	
	for(int i = line*2; i <line*3; i++) {
		vector3.push_back(vector.at(i));
	}
	
	for(int i = line*3; i < vector.size(); i++){
		vector4.push_back(vector.at(i));
	}
	const int kThreadCount {4};
	pthread_t arrThreads[kThreadCount];

	pthread_create(&arrThreads[0],nullptr,&threadRoutine,&vector1);
	pthread_create(&arrThreads[1],nullptr,&threadRoutine,&vector2);
	pthread_create(&arrThreads[2],nullptr,&threadRoutine,&vector3);
	pthread_create(&arrThreads[3],nullptr,&threadRoutine,&vector4);




	//std::list<std::string> listStrContent {this->tr_.getSplitLines(strTextContent)};

	// quarter the file
	//std::vector<std::list<std::string>> veclistStr;
	//this->tr_.quarterList(listStrContent,veclistStr);

	// 3) threading
	

	// create payloads
	//Payload loads[kThreadCount];
	/*for(int i=0;i<kThreadCount;++i){
		//Payload load;
		//PayloadArgs pArgs;
		//pArgs.setSearchSection(veclistStr[i]);
		//pArgs.setTarget(search_phrase);
		//load.args_ = pArgs;
		//loads[i] = load;
	}*/

	
	/*for(int i=0;i<kThreadCount;++i){
		// create payload with the args
		//
		if(pthread_create(&arrThreads[i],nullptr,&threadRoutine,veclis&tStr[i]) != 0){ //pass vector pthread
			perror("thread create");
			exit(1);
		}
	}*/

	// then wait for them to join back
	for(int i=0;i<kThreadCount;++i){
		if(pthread_join(arrThreads[i],nullptr) != 0){
			perror("bad join");
			exit(2);
		}
	}

	// print the hits
	int totalCount {1};
	strTextContent = "";
	for (int i = 0; i< vector1.size(); i++){
		strTextContent += vector1.at(i) + "\n";
			}
	for (int i = 0; i< vector2.size(); i++){
		strTextContent += vector2.at(i) + "\n";
			}
	for (int i = 0; i< vector3.size(); i++){
		strTextContent += vector3.at(i) + "\n";
			}
	for (int i = 0; i< vector4.size(); i++){
		strTextContent += vector4.at(i) + "\n";
			}
	/*for(int i=0;i<kThreadCount;++i){
		for(auto const &str:veclistStr[i]){
			strTextContent += str;
			std::cout << strTextContent << std::endl;
			totalCount ++;
		}
	}*/

	//write string back to addr
	memset(addr , 0 , strTextContent.size());
	strncpy(addr, strTextContent.c_str(),strlen(strTextContent.c_str()));
	msync(addr, strTextContent.size(), MS_SYNC);
	close(fd);

}


#endif