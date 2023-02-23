#ifndef SHARED_MEM_CC
#define SHARED_MEM_CC


#include "./shm.h"

#include <string.h> //memcpy
#include <vector>
#include "../reader/text_reader.h"

// write int to shared mem
void MyShm::writeInt(const int &n){
	this->shmp_->shared_int_ = n;



	if (sem_post(&shmp_->sem1_) == -1)
		errExit("sem_post");

	//Wait until peer says that it has finished accessing the shared memory.

	if (sem_wait(&shmp_->sem2_) == -1)
		errExit("sem_wait");
}

// read an int from shared mem
void MyShm::readInt(int &overwrite_int){
	//Wait for 'sem1' to be posted by peer before touching shared memory.

	if (sem_wait(&shmp_->sem1_) == -1)
		errExit("sem_wait");

	overwrite_int = this->shmp_->shared_int_;

	//Post 'sem2' to tell the peer that it can now access the modified data in shared memory.

	if (sem_post(&shmp_->sem2_) == -1)
		errExit("sem_post");
}

// read str from shared mem buffer
void MyShm::readStr(std::string &overwrite_str){
	// read the number of times to expect str
	int recieveCount {-1};
	this->readInt(recieveCount);

	std::vector<std::string> totalData{};
	std::string currStrData;
	for(int i=0;i<recieveCount;++i){
		//Wait for 'sem1' to be posted by peer before touching shared memory. 

		if (sem_wait(&shmp_->sem1_) == -1)
			errExit("sem_wait");

		currStrData = shmp_->buf_;
		totalData.push_back(currStrData);

		//Post 'sem2' to tell the peer that it can now access the modified data in shared memory.

		if (sem_post(&shmp_->sem2_) == -1)
			errExit("sem_post");
		}
		std::string final_str;
		for(int i=0;i<totalData.size();++i){
			final_str += totalData[i];
		}
		overwrite_str = final_str;
}

// write str to shared mem buffer
void MyShm::writeStr(const std::string &str){
	int BUF_SIZE = 1028;

    this->shmp_->cnt_ = BUF_SIZE;

	int strLen = str.length();

	int sendCount = strLen / BUF_SIZE;
	// if remainder, still add one
	if(strLen % BUF_SIZE >0){sendCount += 1;}

	// first send number of times to expect to read
	this->writeInt(sendCount);

	int currCharIndex {0};
	char c_str_convert [BUF_SIZE];
	
	// now loop for send count and send
	for(int i=0;i<sendCount;++i){
		// create string fragments
		for(int j=0;j<BUF_SIZE;++j){
			if(currCharIndex + j >= strLen){
				// done
				c_str_convert[j] = '\0';
			}else{
				c_str_convert[j] = str[currCharIndex+j];
			}
		}
		memcpy(&shmp_->buf_,c_str_convert,this->shmp_->cnt_);

		/* Tell peer that it can now access shared memory. */

		if (sem_post(&shmp_->sem1_) == -1)
			errExit("sem_post");

		/* Wait until peer says that it has finished accessing
			the shared memory. */

		if (sem_wait(&shmp_->sem2_) == -1)
			errExit("sem_wait");
		currCharIndex += BUF_SIZE;
	}
}

#endif