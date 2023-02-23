#ifndef UNIX_DOMAIN_SOCKET_CC
#define UNIX_DOMAIN_SOCKET_CC

#include "./unix_domain_socket.h"
#include <algorithm>



UnixDomainSocket::UnixDomainSocket(){

    const char * c = this->socket_path_.c_str();

	
	sock_addr_ = {};
	
	sock_addr_.sun_family = AF_UNIX;

	
	strncpy(sock_addr_.sun_path + 1, // use strncpy to limit copy for portability
		c,
		sizeof(sock_addr_.sun_path) - 2); // -2 for leading / trailing 0s
    
	
	this->buff_ = new char[this->buff_size_];

}



int UnixDomainSocket::sendInt(const int &sock_fd, const int &n){
	// need to write int to buff
	//
	int send_data = n;
	// return the bytes sent
	//
	return ::send(sock_fd,&send_data,sizeof(int),0);
}


int UnixDomainSocket::readInt(const int &sock_fd,int &overwrite_int){
	// return bytes read
	//
	return ::recv(sock_fd,&overwrite_int,sizeof(int),0);
}


void UnixDomainSocket::sendStr(const int&sock_fd, const std::string &msg){
	// need to make sure str can fit
	// send at most BUFF_SIZE size at time
	// split them up and build it back.
  
    int intMsgLength = msg.length();

	// for now just an arbitrary number of chars
    int intSendCount = intMsgLength / this->buff_size_;
    int intRemainderCharCount = intMsgLength %this->buff_size_;
    if(intRemainderCharCount != 0){
        intSendCount += 1;
    }

	// send the number of times the client should expect to listen
    this->sendInt(sock_fd,intSendCount);

    int currCharIndex {0};
    char c_str_convert [this->buff_size_];
    // std::cout << "size of msg: " << intMsgLength << "\n";
    // keep sending up to this->buff_size number of chars
    for(int i=0;i<intSendCount;++i){
        

        for(int j =0;j<this->buff_size_;++j){
            if(currCharIndex + j >= intMsgLength){
                // special character
                c_str_convert[j] = this->DELIM_;
            }else{
                c_str_convert[j] = msg[currCharIndex+j];
            }
            
        }

		// send the characters
        ::send(sock_fd,c_str_convert,this->buff_size_*sizeof(char),0);

        // increment the currenct character index to match
        currCharIndex += this->buff_size_;

    }
}


int UnixDomainSocket::readStr(const int &sock_fd,std::string &overwrite_str){
	int intRecieveCount {-1};
    // get the number of characters first
    this->readInt(sock_fd,intRecieveCount);
	// std::cout << "need to recieve " << intRecieveCount << " times\n";

    // std::cout << "Need to recieve " << intRecieveCount << " times\n";
    
    std::vector<std::string> totalData {};
    std::string currStrData;

    for(int i=0;i<intRecieveCount;++i){
        // keep printing strings to show it works
        ::recv(sock_fd,this->buff_,this->buff_size_*sizeof(char),0);
        currStrData = &this->buff_[0];
        // remove \a characters
        // currStrData.erase(remove(currStrData.begin(),currStrData.end(),'\a'),currStrData.end());

        // append to vector
        totalData.push_back(currStrData);
    }
    std::string final_str;

    for(int i=0;i<totalData.size();++i){
        final_str += totalData[i];
    }
    overwrite_str = final_str;

	return overwrite_str.size();
}


void UnixDomainSocket::sendListStr(const int& sock_fd, const std::list<std::string> str_list){
    // first send size of list
    int list_size = str_list.size();

    // send int
    sendInt(sock_fd,list_size);

    // now for loop to send
    //
    for(auto const& i : str_list){
        sendStr(sock_fd,i);
    }

}

void UnixDomainSocket::readListStr(const int &sock_fd,std::list<std::string> &overwrite_list){
    std::list<std::string> temp_list {};
    // first read size of list
    int list_size {-1};

    readInt(sock_fd,list_size);

    std::string overwrite_str{};
    for(int i=0;i<list_size;++i){
        readStr(sock_fd,overwrite_str);
        temp_list.push_back(overwrite_str);
    }

    // overwrite the list
    overwrite_list = temp_list;
}


#endif