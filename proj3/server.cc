#include "./ram/sock/domain_socket_server.h"

int main(int argc, char* argv[]){

	DomainSocketServer dss;
	
	dss.runServer();

	return 0;

}