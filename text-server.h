class DomainSocketServer {
 public:
  DomainSocketServer(const char *socket_path);
  void RunServer();

 protected:
  ::sockaddr_un sock_addr_;  // socket address from sys/un.h
  std::string socket_path_;  // let std::string manage char *
};
