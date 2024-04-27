#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <unordered_set>
#include "message.h"
#include "csapp.h"
#include "value_stack.h"

class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server;
  int m_client_fd;
  rio_t m_fdbuf;
  ValueStack stack;
  std::unordered_set<Table*> locked_tables;
  bool in_transaction;

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );


public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();

  int get_m_client_fd();


private:
  /* Takes an exception and turns it into a message to send to the client, then kills the 
  ClientConnection if the error is unrecoverable. */
  void manage_exception(std::runtime_error ex, bool recoverable);

  /* Rolls back changes in altered Tables and unlocks them. */
  void fail_transaction();

  /* Finds a message's type and calls the appropriate response function based on the type. */
  void call_response_function(Message client_msg);

  void handle_begin();

  void handle_commit();

  void handle_pop();

  void handle_top();

  void handle_add();

  void handle_sub();

  void handle_mul();

  void handle_div();

  void handle_bye();

  void handle_login(bool first_valid_message);

  void handle_create(Message client_msg);

  void handle_push(Message client_msg);

  void handle_set(Message client_msg);

  void handle_get(Message client_msg);

  /* Respond to client with OK Message */
  void write_ok();
};

#endif // CLIENT_CONNECTION_H
