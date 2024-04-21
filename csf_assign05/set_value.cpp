#include <iostream>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

using namespace MessageSerialization;


/* Handle a response that is expected to be OK. Return 0 for OK, -1 otherwise. */
int handle_possible_ok_response(char* buf) {
  Message response;
  decode(buf, response);
  MessageType response_type = response.get_message_type();

  // If the server responded with OK, continue the program
  if(response_type == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(response_type == MessageType::FAILED || response_type == MessageType::ERROR) {

    std::string error_message = response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK Message or an error Message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}


int login_operation(std::string username, int fd) {

  // Create a login Message
  Message login_msg(MessageType::LOGIN);
  login_msg.push_arg(username);

  // Encode and send the Message
  std::string encoded_login;
  encode(login_msg, encoded_login);
  rio_writen(fd, encoded_login.data(), encoded_login.size());

  // Read the server's response
  rio_t read;
  rio_readinitb(&read, fd);
  char buf[login_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // Return with -1 if the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  // Handle the OK, error or undefined response
  return handle_possible_ok_response(buf);
}


int push_operation(std::string value, int fd) {

  // Create a PUSH Message
  Message push_msg(MessageType::PUSH);
  push_msg.push_arg(value);

  // Encode and print the Message
  std::string encoded_push;
  encode(push_msg, encoded_push);
  rio_writen(fd, encoded_push.data(), encoded_push.size());

  // Read the server's response
  rio_t read;
  rio_readinitb(&read, fd);
  char buf[push_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  return handle_possible_ok_response(buf);
}


int set_operation(std::string table, std::string key, int fd) {

  // Create a SET Message
  Message set_msg(MessageType::SET);
  set_msg.push_arg(table);
  set_msg.push_arg(key);

  // Encode and print the Message
  std::string encoded_set;
  encode(set_msg, encoded_set);
  rio_writen(fd, encoded_set.data(), encoded_set.size());

  // Read the server's response
  rio_t read;
  rio_readinitb(&read, fd);
  char buf[set_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }

  return handle_possible_ok_response(buf);
}


int bye_operation(int fd) {
  Message bye_msg(MessageType::BYE);
  std::string encoded_bye;

  encode(bye_msg, encoded_bye);
  rio_writen(fd, encoded_bye.data(), encoded_bye.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[bye_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  return handle_possible_ok_response(buf);
}


int main(int argc, char **argv)
{
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];
  std::string value = argv[6];

  // Try to connect to server
  int fd = open_clientfd(hostname.data(), port.data());
  if (fd < 0) {
    std::cerr << "Error: Could not connect to server.\n";
    return 1;
  }

  /* LOGIN operation. Errors printed in functions */
  int login_result = login_operation(username, fd);
  if (login_result != 0) {
    return 1;
  }

  /* PUSH operation. Errors printed in functions */
  int push_result = push_operation(value, fd);
  if (push_result != 0) {
    return 1;
  }

   /* SET operation. Errors printed in functions */
  int set_result = set_operation(table, key, fd);
  if (set_result != 0) {
    return 1;
  } 

  /* BYE operation. Errors/output printed in functions. */
  int bye_result = bye_operation(fd);
  if (bye_result != 0) {
    return 1;
  }

  return 0;
}
