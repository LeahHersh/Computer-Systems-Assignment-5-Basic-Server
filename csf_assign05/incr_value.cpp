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


int begin_operation(int fd) {
  Message begin_msg(MessageType::BEGIN);
  std::string encoded_begin;

  encode(begin_msg, encoded_begin);
  rio_writen(fd, encoded_begin.data(), encoded_begin.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[begin_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  Message begin_response;
  decode(buf, begin_response);

  // If the server responded with OK, continue the program
  if(begin_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(begin_response.get_message_type() == MessageType::FAILED || 
          begin_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = begin_response.get_arg(0);
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


int push_operation(int fd) {

  Message push_msg(MessageType::PUSH);
  push_msg.push_arg("1");
  std::string encoded_push;

  encode(push_msg, encoded_push);
  rio_writen(fd, encoded_push.data(), encoded_push.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[push_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  Message push_response;
  decode(buf, push_response);

  // If the server responded with OK, continue the program
  if(push_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(push_response.get_message_type() == MessageType::FAILED || 
          push_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = push_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}


int add_operation(int fd) {

  Message add_msg(MessageType::ADD);
  std::string encoded_add;

  encode(add_msg, encoded_add);
  rio_writen(fd, encoded_add.data(), encoded_add.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[add_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  Message add_response;
  decode(buf, add_response);

  // If the server responded with OK, continue the program
  if(add_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(add_response.get_message_type() == MessageType::FAILED || 
          add_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = add_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}


int set_operation(std::string table, std::string key, int fd) {
  Message set_msg(MessageType::SET);
  set_msg.push_arg(table);
  set_msg.push_arg(key);
  std::string encoded_set;

  encode(set_msg, encoded_set);
  rio_writen(fd, encoded_set.data(), encoded_set.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[set_msg.MAX_ENCODED_LEN];
  int n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }

  Message set_response;
  decode(buf, set_response);

  // If the server responded with OK, continue the program
  if(set_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(set_response.get_message_type() == MessageType::FAILED || 
          set_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = set_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}


int commit_operation(int fd) {
  Message commit_msg(MessageType::COMMIT);
  std::string encoded_commit;

  encode(commit_msg, encoded_commit);
  rio_writen(fd, encoded_commit.data(), encoded_commit.size());

  rio_t read;
  rio_readinitb(&read, fd);
  char buf[commit_msg.MAX_ENCODED_LEN];
  ssize_t n = rio_readlineb(&read, buf, sizeof(buf));

  // If the server response couldn't be read
  if (n <= 0) {
    std::cerr << "Error: could not read server's response.\n";
    return -1;
  }
  
  Message commit_response;
  decode(buf, commit_response);

  // If the server responded with OK, continue the program
  if(commit_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(commit_response.get_message_type() == MessageType::FAILED || 
          commit_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = commit_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
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
  
  Message bye_response;
  decode(buf, bye_response);

  // If the server responded with OK, continue the program
  if(bye_response.get_message_type() == MessageType::OK) {
    return 0;
  }
  // If the server responded with an error message
  else if(bye_response.get_message_type() == MessageType::FAILED || 
          bye_response.get_message_type() == MessageType::ERROR) {

    std::string error_message = bye_response.get_arg(0);
    std::cerr << "Error: " << error_message << std::endl;
    return -1;
  }

  // If the server didn't respond with OK or an error message
  std::cerr << "Error: confirmation from server not received.\n";
  return -1;
}


int main(int argc, char **argv) {
  if ( argc != 6 && (argc != 7 || std::string(argv[1]) != "-t") ) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -t      execute the increment as a transaction\n";
    return 1;
  }

  int count = 1;

  bool use_transaction = false;
  if ( argc == 7 ) {
    use_transaction = true;
    count = 2;
  }

  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];

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

  /* BEGIN operation. Errors printed in functions. Only runs when 7 arguments are entered. */
  if (use_transaction) {
    int begin_result = begin_operation(fd);
    if (begin_result != 0) {
      return 1;
    }
  }

  /* GET operation. Errors printed in functions */
  int get_result = get_operation(table, key, fd);
  if (get_result != 0) {
    return 1;
  }

  /* PUSH operation. Errors printed in functions */
  int push_result = push_operation(fd);
  if (push_result != 0) {
    return 1;
  }

  /* ADD operation. Errors printed in functions */
  int add_result = add_operation(fd);
  if (add_result != 0) {
    return 1;
  } 

  /* SET operation. Errors printed in functions */
  int set_result = set_operation(table, key, fd);
  if (set_result != 0) {
    return 1;
  } 

  /* COMMIT operation. Errors printed in functions. Only runs when 7 arguments are entered. */
  if (use_transaction) {
    int commit_result = commit_operation(fd);
    if (commit_result != 0) {
      return 1;
    }
  }

  /* BYE operation. Errors/output printed in functions. */
  int bye_result = bye_operation(fd);
  if (bye_result != 0) {
    return 1;
  }

  return 0;
}
