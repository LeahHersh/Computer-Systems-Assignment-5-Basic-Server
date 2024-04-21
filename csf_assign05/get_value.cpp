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


/* Handle a response that is expected to be DATA. Return 0 for DATA, -1 otherwise. */
int handle_top_response(char* buf) {
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
  else if (top_response.get_message_type() == MessageType::ERROR || top_response.get_message_type() == MessageType::FAILED) {

    std::string error_message = top_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with DATA or an error message
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


int get_operation(std::string table, std::string key, int fd) {

  // Create a GET Message
  Message get_msg(MessageType::GET);
  get_msg.push_arg(table);
  get_msg.push_arg(key);

  // Encode and send the Message
  std::string encoded_get;
  encode(get_msg, encoded_get);
  rio_writen(fd, encoded_get.data(), encoded_get.size());

  // Read the server's response
  rio_t read;
  rio_readinitb(&read, fd);
  char buf[get_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }

  return handle_possible_ok_response(buf);
}


int top_operation(int fd) {

  // Create a TOP Message
  Message top_msg(MessageType::TOP);

  // Encode and send the Message
  std::string encoded_top;
  encode(top_msg, encoded_top);
  rio_writen(fd, encoded_top.data(), encoded_top.size());

  // Read the server's response
  rio_t read;
  rio_readinitb(&read, fd);
  char buf[top_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  return handle_top_response(buf);
}


int bye_operation(int fd) {

  // Create BYE Message
  Message bye_msg(MessageType::BYE);

  // Encode and send Message
  std::string encoded_bye;
  encode(bye_msg, encoded_bye);
  rio_writen(fd, encoded_bye.data(), encoded_bye.size());

  // Read server's response
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
  int fd = open_clientfd(hostname.data(), port.data());
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
  
  /* BYE operation. Errors/output printed in functions. */
  int bye_result = bye_operation(fd);
  if (bye_result != 0) {
    return 1;
  }

  return 0;
}
