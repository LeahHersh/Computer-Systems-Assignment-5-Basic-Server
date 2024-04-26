#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

using namespace MessageSerialization;

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
  , in_transaction(false)  {
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection() {

  if (in_transaction) {
    fail_transaction();
  }

  close(m_client_fd);
}


void ClientConnection::chat_with_client() {
  bool input_left = true;
  bool first_valid_message = true;
  Message response_msg;
  char buf[response_msg.MAX_ENCODED_LEN];

  while (input_left) {
    ssize_t n = Rio_readlineb(&m_fdbuf, buf, sizeof(m_fdbuf));

    // If nothing is read from the client
    if (n <= 0) { input_left = false; }
    else {
      try { decode(buf, response_msg); } 
      catch (InvalidMessage ex) { manage_exception(ex, false); }
      
      // If the Message is a login
      if (response_msg.get_message_type() == MessageType::LOGIN) {
        handle_login(first_valid_message);
        continue;
        // If the Message isn't a login but is the first valid Message
      } else if (first_valid_message) {
        Message error_msg (MessageType::FAILED, { "Please log in first." });
        std::string encoded_error;
        encode(error_msg, encoded_error);
        rio_writen(m_client_fd, encoded_error.data(), encoded_error.size());
        continue;
      }
      first_valid_message = false;

      call_response_function(response_msg);
    }
    // End connection with client when no more input can be read
    free(this);
  }
}


int ClientConnection::get_m_client_fd() {
  return m_client_fd;
}


void ClientConnection::manage_exception (std::runtime_error ex, bool recoverable) {

  // Assign a MessageType to the server's response to the client
  MessageType response_type = (recoverable) ? MessageType::FAILED : MessageType::ERROR;

  // Finish creating and encode the Message
  Message response (response_type, { ex.what() });
  std::string encoded_response;
  encode(response, encoded_response);

  // Send the Message
  rio_writen(m_client_fd, encoded_response.data(), encoded_response.size());

  // Fail ongoing transaction if necessary 
  if (in_transaction) { fail_transaction(); }

  // End the ClientConnection if necessary
  if (response_type == MessageType::ERROR) { free(this); }
}


void ClientConnection::fail_transaction() {
  for (auto it = locked_tables.begin(); it != locked_tables.end(); it++) {
    (*it)->rollback_changes();
    (*it)->unlock();
  }

  locked_tables.clear();
}


void ClientConnection::call_response_function(Message response_msg) {
  MessageType response_type = response_msg.get_message_type();
  
  try {
    // Choose which helper function to call
    switch (response_type) {

      case MessageType::CREATE:
        handle_create(response_msg);
        break;
      case MessageType::BEGIN:
        handle_begin(response_msg);
        break;
      case MessageType::COMMIT:
        handle_commit(response_msg);
        break;
      case MessageType::POP:
        handle_pop(response_msg);
        break;
      case MessageType::TOP:
        handle_top(response_msg);
        break;
      case MessageType::ADD:
        handle_add(response_msg);
        break;
      case MessageType::SUB:
        handle_sub(response_msg);
        break;
      case MessageType::MUL:
        handle_mul(response_msg);
        break;
      case MessageType::DIV:
        handle_div(response_msg);
        break;
      case MessageType::BYE:
        handle_bye(response_msg);
        break;
      case MessageType::PUSH:
        handle_push(response_msg);
        break;
      case MessageType::SET:
        handle_set(response_msg);
        break;
      case MessageType::GET:
        handle_get(response_msg);
        break;
      default: throw OperationException("Please only enter standardized requests.");
    }
  }

  catch (CommException ex) {      manage_exception(ex, false); }
  catch (std::runtime_error ex) { manage_exception(ex, true); }
}


void ClientConnection::handle_create(Message response_msg) {

  m_server->lock();

  std::string table_name = response_msg.get_arg(0);
  std::unordered_map<std::string, Table*> server_map = m_server->get_table_map();

  // If the table's names is already in the server's map of tables
  if (server_map.find(table_name) != server_map.end()) {
    throw OperationException("A table with this name already exists.");
  }

  m_server->create_table(table_name);
  write_ok();

  m_server->unlock();
}


void ClientConnection::handle_login(bool first_valid_message) {
  
  if(!first_valid_message) {
    throw OperationException("You are already logged in.");
  }

  write_ok();
}


void ClientConnection::handle_begin(Message response_msg) {

  if (in_transaction) {
    throw FailedTransaction("Nested transactions are not supported.");
  }

  in_transaction = true;
  write_ok();
}


void ClientConnection::handle_commit(Message response_msg) { 

  if (!in_transaction) {
    throw FailedTransaction("Transaction is not ongoing.");
  }

  // Otherwise, commit all changes and unlock used tables.
  for (auto it = locked_tables.begin(); it != locked_tables.end(); it++) {
    (*it)->commit_changes();
    (*it)->unlock();
  }
  locked_tables.clear();
  in_transaction = false;

  write_ok();
}


void ClientConnection::write_ok() {
  // Create OK Message
  Message ok_msg(MessageType::OK);

  // Encode and send Message
  std::string encoded_ok;
  encode(ok_msg, encoded_ok);
  rio_writen(m_client_fd, encoded_ok.data(), encoded_ok.size());
}

