#include <iostream>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

using namespace MessageSerialization;

int login_operation(std::string username, int fd) {

  /* Login operation */
  Message login_msg(MessageType::LOGIN);
  login_msg.push_arg(username);
  std::string encoded_login;

  encode(login_msg, encoded_login);
  rio_writen(fd, encoded_login.data(), encoded_login.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[login_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";

    return -1;
  }
  
  Message login_response;
  decode(buf, login_response);

  // If the server didn't respond with OK, exit
  if(login_response.get_message_type() != MessageType::OK) {
    std::cerr << "Error: confirmation from server not recieved.\n";
    return -1;
  }

  return 0;
}


int get_operation(std::string table, std::string key, int fd) {
  Message get_msg(MessageType::GET);
  get_msg.push_arg(table);
  get_msg.push_arg(key);
  std::string encoded_get;

  encode(get_msg, encoded_get);
  std::cerr<< encoded_get;
  rio_writen(fd, encoded_get.data(), encoded_get.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[get_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    close(fd);
    return -1;
  }

  Message get_response;
  decode(buf, get_response);

  // If the server doesn't respond with OK, exit
  if(get_response.get_message_type() != MessageType::OK) {
    std::cerr << "Error: confirmation from server not recieved.\n";
    return -1;
  }

  return 0;
}


int top_operation(int fd) {
  Message top_msg(MessageType::TOP);
  std::string encoded_top;

  encode(top_msg, encoded_top);
  std::cerr<< encoded_top;
  rio_writen(fd, encoded_top.data(), encoded_top.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[top_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    close(fd);
    return -1;
  }
  
  Message top_response;
  decode(buf, top_response);

  // If the server doesn't respond with a DATA message with one param, exit
  if(top_response.get_message_type() != MessageType::DATA) {
    return -1;
  }

  return 0;
}


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

  /* LOGIN operation. Errors printed in functions. */
  int login_result = login_operation(username, fd);
  if (login_result != 0) {
    return 1;
  }

  /* GET operation. Errors printed in functions. */
  int get_result = get_operation(table, key, fd);
  if (get_result != 0) {
    return 1;
  }
  
  /* TOP operation */
  int top_result = top_operation(fd);
  if (top_result != 0) {
    return 1;
  }
  
}
