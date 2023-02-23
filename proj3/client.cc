#include "./ram/sock/domain_socket_client.h"

int main(int argc, char* argv[]){

	if(argc != 3){
		std::cout << "text-client [FILE PATH] [TARGET STRING]\n";
		return 1;
	}
	else{
		
		std::string file_path = argv[1];

		std::string search_phrase = argv[2];

		DomainSocketClient dsc;
		dsc.runClient(file_path,search_phrase);

	}

	
	return 0;

}