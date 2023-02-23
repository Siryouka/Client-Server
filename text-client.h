class DomainSocketClient {
 public:
  explicit DomainSocketClient(const char *socket_path, const char * file_path, const char* searchstring);
  void RunClient();

 protected:
  ::sockaddr_un sock_addr_;  // socket address from sys/un.h

  std::string socket_path_;  // let std::string manage char *
  std::string file_name_; // let std::string manage char *
  std::string search_string_; // let std::string manage char *
};