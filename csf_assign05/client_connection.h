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

  /* Takes an exception and turns it into a failure response to client, then kills the client. */
  void manage_unrecoverable_ex(std::runtime_error ex);

  /* Finds a message's type and calls the appropriate response function based on the type. */
  void call_response_function(Message response_msg);

  void handle_begin(Message response_msg);

  void handle_commit(Message response_msg);

  void handle_pop(Message response_msg);

  void handle_top(Message response_msg);

  void handle_add(Message response_msg);

  void handle_sub(Message response_msg);

  void handle_mul(Message response_msg);

  void handle_div(Message response_msg);

  void handle_bye(Message response_msg);

  void handle_login(bool first_valid_message);

  void handle_create(Message response_msg);

  void handle_push(Message response_msg);

  void handle_set(Message response_msg);

  void handle_get(Message response_msg);

  /* Respond to client with OK Message */
  void write_ok();

  int get_m_client_fd();

  // TODO: additional member functions
};

#endif // CLIENT_CONNECTION_H
