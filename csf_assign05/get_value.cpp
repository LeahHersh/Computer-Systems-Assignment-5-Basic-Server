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

  // If the server responded with OK, continue the program
  if(login_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(login_response.get_message_type() == MessageType::FAILED || 
          login_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = login_response.get_arg(0);

    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}


int get_operation(std::string table, std::string key, int fd) {
  Message get_msg(MessageType::GET);
  get_msg.push_arg(table);
  get_msg.push_arg(key);
  std::string encoded_get;

  encode(get_msg, encoded_get);
  rio_writen(fd, encoded_get.data(), encoded_get.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[get_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }

  Message get_response;
  decode(buf, get_response);

  // If the server responded with OK, continue the program
  if(get_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(get_response.get_message_type() == MessageType::FAILED || 
          get_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = get_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}


int top_operation(int fd) {
  Message top_msg(MessageType::TOP);
  std::string encoded_top;

  encode(top_msg, encoded_top);
  rio_writen(fd, encoded_top.data(), encoded_top.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[top_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  Message top_response;
  decode(buf, top_response);

  // If the server responds with a DATA message
  if(top_response.get_message_type() == MessageType::DATA) {

    // If the DATA response does not contain a parameter
    if (top_response.get_num_args() < 1) {
      std::cerr << "Error: response does not contain necessary information.\n";
      return -1;
    }
  
    std::string data = top_response.get_arg(0);
    std::cout << data << std::endl;
    return 0;
  }
  // Or the server responds with an error message
  else if (top_response.get_message_type() == MessageType::ERROR ||
           top_response.get_message_type() == MessageType::FAILED) {

    std::string error_message = top_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with DATA or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}

void bye_operation(int fd) {
  Message bye_msg(MessageType::BYE);
  std::string encoded_bye;

  encode(bye_msg, encoded_bye);
  rio_writen(fd, encoded_bye.data(), encoded_bye.size());
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
  
  /* TOP operation. Errors/output printed in functions. */
  int top_result = top_operation(fd);
  if (top_result != 0) {
    return 1;
  }
  
  bye_operation(fd);
  return 0;
}
