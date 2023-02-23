#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <string>
#include <iostream>

#include "text-client.h"

// client needs domain socket file path search string
DomainSocketClient::DomainSocketClient(const char *socket_path, const char * file_path, const char* searchstring) {
  socket_path_ = std::string(socket_path);  // std::string manages char *

  sock_addr_ = {};  // init struct (replaces memset)
  sock_addr_.sun_family = AF_UNIX;  // set to Unix domain socket (e.g. instead
                                    //   of internet domain socket)
  // leaving leading null char sets abstract socket
  strncpy(sock_addr_.sun_path + 1,  // use strncpy to limit copy for
          socket_path,              //   portability
          sizeof(sock_addr_.sun_path) - 2);  // -2 for leading/trailing \0s

  file_name_ = std::string(file_path);
  search_string_ = std::string(searchstring);

}


void DomainSocketClient::RunClient() {
  // (1) open nameless Unix socket
  int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  // (2) connect to an existing socket
  int success = connect(socket_fd,
                        // sockaddr_un is a Unix sockaddr
                        reinterpret_cast<const sockaddr*>(&sock_addr_),
                        sizeof(sock_addr_));
  if (success < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  std::clog << "SERVER CONNECTION ACCEPTED" << std::endl;

  // (3) write to socket
  const ssize_t kWrite_buffer_size = 4096;
  char write_buffer[kWrite_buffer_size];
  char recv_buffer[kWrite_buffer_size];
  const char kKill_msg[] = "quit";
  const char kRecv_msg[] = "recv";
  

  // (4) Send file_name to server, wait for server to acknowledege
  ssize_t bytes_wrote = write(socket_fd, file_name_.c_str(), file_name_.length());
  while (true) {
    ssize_t bytes_read = read(socket_fd, recv_buffer, kWrite_buffer_size);
    if (strcmp(recv_buffer, kRecv_msg) == 0){
      break;
    }
  }

  // (5) Send serach_string to server, wait for srever to acknowledge
  bytes_wrote = write(socket_fd, search_string_.c_str(), search_string_.length());
  while (true) {
    ssize_t bytes_read = read(socket_fd, recv_buffer, kWrite_buffer_size);
    if (strcmp(recv_buffer, kRecv_msg) == 0){
      break;
    }
  }

  // (6) Wait for server to send matched lines in the file
  ssize_t bytes_read = read(socket_fd, recv_buffer, kWrite_buffer_size);
  int count = 0;
  ssize_t total_bytes_read = 0;
  while (bytes_read > 0) {
    if (strcmp(recv_buffer, kKill_msg) == 0) {
      // std::cout << "Server shutting down..." << std::endl;

      bytes_read = 0;  // message handled, disconnect client
      break;
    }

    // std::cout << "read " << bytes_read << " bytes: ";
    // std::cout.write(recv_buffer, bytes_read) << std::endl;
    count++;
    std::cout << count <<  "\t" << std::string(recv_buffer,bytes_read) << std::endl;
  
    write(socket_fd, kRecv_msg, sizeof(kRecv_msg));
    total_bytes_read += bytes_read;
    bytes_read = read(socket_fd, recv_buffer, kWrite_buffer_size);
  }
  // (7) Print out total bytes received
  std::clog << "BYTES RECEIVED: " << total_bytes_read << std::endl;

  if (bytes_read == 0) {
    // std::cout << "Client disconnected" << std::endl;

    close(socket_fd);
  } else if (bytes_read < 0) {
    std::cerr << strerror(errno) << std::endl;

    exit(-1);
  }

}



int main(int argc, char *argv[]) {
  if (argc != 4)
    return 1;

    DomainSocketClient dsc(argv[1], argv[2], argv[3]);
    dsc.RunClient();

  return 0;
}