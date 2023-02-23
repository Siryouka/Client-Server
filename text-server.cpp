#include <sys/socket.h>  // Unix header for sockets, using socket
#include <sys/un.h>  // defns for Unix domain sockets, using struct sockaddr_un
#include <unistd.h>  // Unix standard header, using close
#include <sys/sysinfo.h> // For get_nprocs_conf()

#include <cassert>  // using assert
#include <cerrno>  // using errno
#include <cstddef>  // using size_t
#include <cstdlib>  // exit
#include <cstring>  // using strncpy, strerror

#include <string>
#include <iostream>
#include <fstream>

#include "text-server.h"

DomainSocketServer::DomainSocketServer(const char *socket_path) {
  socket_path_ = std::string(socket_path);  // std::string manages char *

  sock_addr_ = {};  // init struct (replaces memset)
  sock_addr_.sun_family = AF_UNIX;  // set to Unix domain socket (e.g. instead
                                    //   of internet domain socket)
  // leaving leading null char sets abstract socket
  strncpy(sock_addr_.sun_path + 1,  // use strncpy to limit copy for
          socket_path,              //   portability
          sizeof(sock_addr_.sun_path) - 2);  // -2 for leading/trailing \0s
}

void DomainSocketServer::RunServer() {
  int sock_fd;  // unnamed socket file descriptor
  int client_req_sock_fd;  // client connect request socket file descriptor

  // (1) create a socket
  //       AF_UNIX -> file system pathnames
  //       SOCK_STREAM -> sequenced bytestream
  //       0 -> default protocol (let OS decide correct protocol)
  sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if ( sock_fd < 0 ) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  // (2) bind socket to address for the server
  unlink(socket_path_.c_str());  // sys call to delete file if it exists
                                  // already using Unix system calls for
                                  //   sockets, no reason to be non-Unix
                                  //   portable now.  :-/
  int success = bind(sock_fd,
                      // sockaddr_un is a Unix sockaddr and so may be cast "up"
                      //   to that pointer type (think of it as C polymorphism)
                      reinterpret_cast<const sockaddr*>(&sock_addr_),
                      // size needs be known due to underlying data layout,
                      //   i.e., there may be a size difference between parent
                      //   and child
                      sizeof(sock_addr_));
  if (success < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  // (3) Listen for connections from clients
  size_t kMax_client_conns = get_nprocs_conf()-1;
  success = listen(sock_fd, kMax_client_conns);
  if (success < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  std::clog << "SERVER STARTED" << std::endl;
  std::clog << "\tMAX CLIENTS: " << get_nprocs_conf()-1<< std::endl;

  const size_t kRead_buffer_size = 64;
  char read_buffer[kRead_buffer_size];
  int bytes_read;
  while (true) {
    // (4) Accept connection from a client
    client_req_sock_fd = accept(sock_fd, nullptr, nullptr);
    if (client_req_sock_fd  < 0) {
      std::cerr << strerror(errno) << std::endl;
      continue;
    }

    std::clog << "CLIENT CONNECTED" << std::endl;

    const char kKill_msg[] = "quit";  // TODO(lewisjs): trim whitespace
                                      //   from read_buffer for comparison
    const char kRecv_msg[] = "recv";
    const char kInvalidFile_msg[] = "INVALID FILE";

    // (5) Receive file_name from client
    bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
    ssize_t bytes_wrote = write(client_req_sock_fd, kRecv_msg, sizeof(kRecv_msg));
    std::string file_name_;
    file_name_.append(read_buffer,bytes_read);
    std::clog << "PATH: " << file_name_ << std::endl;

    // (6) Receive search_string from client
    bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
    std::string search_string_;
    search_string_.append(read_buffer,bytes_read);
    bytes_wrote = write(client_req_sock_fd, kRecv_msg, sizeof(kRecv_msg));
    std::clog << "SEEKING: " << search_string_ << std::endl;

    // (7) Read the files
    // https://www.w3schools.com/cpp/cpp_files.asp
    // Create a text string, which is used to output the text file
    std::string myText;
    std::ifstream MyReadFile(file_name_);

    if (!MyReadFile.is_open()){
      write(client_req_sock_fd, kInvalidFile_msg, sizeof(kInvalidFile_msg));
      close(client_req_sock_fd);
    }else{
      // Use a while loop together with the getline() function to read the file line by line
      ssize_t total_bytes_wrote = 0;
      while (getline (MyReadFile, myText)) {
        // Check if line contains search_string_
        // https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
        // If it does, send myText via socket
        if (myText.find(search_string_) != std::string::npos){
          // Send myText
          bytes_wrote = write(client_req_sock_fd, myText.c_str(), myText.length());
          total_bytes_wrote += bytes_wrote;
          // Wait for ack before sending next line
          while (true) {
            ssize_t bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
            if (strcmp(read_buffer, kRecv_msg) == 0){
              break;
            }
          }
        }
      }

      // Close the file
      MyReadFile.close();



      std::clog << "BYTES SENT: " << total_bytes_wrote << std::endl;


      // (8) Send quit signal to client
      bytes_wrote = write(client_req_sock_fd, kKill_msg, sizeof(kKill_msg));


      bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
      while (bytes_read > 0) {
        if (strcmp(read_buffer, kKill_msg) == 0) {
          std::cout << "Server shutting down..." << std::endl;

          bytes_read = 0;  // message handled, disconnect client
          break;
        }

        // std::cout << "read " << bytes_read << " bytes: ";
        // std::cout.write(read_buffer, bytes_read) << std::endl;

        bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
      }

      if (bytes_read == 0) {
        close(client_req_sock_fd);
      } else if (bytes_read < 0) {
        std::cerr << strerror(errno) << std::endl;

        exit(-1);
      }
    }

    
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    return 1;

    DomainSocketServer dss(argv[1]);
    dss.RunServer();
  return 0;
}