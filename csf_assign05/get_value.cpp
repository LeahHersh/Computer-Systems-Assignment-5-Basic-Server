#include <iostream>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

using namespace MessageSerialization;

int main(int argc, char **argv)
{
  if ( argc != 6 ) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  // Try to connect to server
  int fd = open_clientfd(argv[1], argv[2]);
  if (fd < 0) {
    std::cerr << "Error: Could not connect to server.\n";
    return 1;
  }

  /* Login operation */
  Message login_msg(MessageType::LOGIN);
  login_msg.push_arg(username);
  std::string encoded_login = " ";

  encode(login_msg, encoded_login);
  std::cerr<< encoded_login;
  rio_writen(fd, encoded_login.data(), encoded_login.size());
  rio_writen(fd, "\n", 1);

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[1000];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  if (n <= 0) {
    std::cerr << "Error: could not recieve server's response.\n";
    close(fd);
    return 0;
  }
  
}
