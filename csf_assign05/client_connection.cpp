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
  Message client_msg;
  char buf[client_msg.MAX_ENCODED_LEN];

  while (input_left) {
    ssize_t n = rio_readlineb(&m_fdbuf, buf, sizeof(m_fdbuf));

    // If nothing is read from the client
    if (n <= 0) { input_left = false; }
    else {
      try { decode(buf, client_msg); } 
      catch (InvalidMessage const& ex) { manage_exception(ex, false); }
      
      // If the Message is a login
      if (client_msg.get_message_type() == MessageType::LOGIN) {
        try { handle_login(&first_valid_message); }
        catch (OperationException const& ex) { manage_exception(ex, true); }
        continue;
        // If the Message isn't a login but is the first valid Message
      } else if (first_valid_message) {
        Message error_msg (MessageType::FAILED, { "Please log in first." });
        std::string encoded_error;
        encode(error_msg, encoded_error);
        rio_writen(m_client_fd, encoded_error.data(), encoded_error.size());
        continue;
      }

      call_response_function(client_msg);
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


void ClientConnection::call_response_function(Message client_msg) {
  MessageType response_type = client_msg.get_message_type();
  
  try {
    // Choose which helper function to call
    switch (response_type) {

      case MessageType::CREATE:
        handle_create(client_msg);
        break;
      case MessageType::BEGIN:
        handle_begin();
        break;
      case MessageType::COMMIT:
        handle_commit();
        break;
      case MessageType::POP:
        handle_pop();
        break;
      case MessageType::TOP:
        handle_top();
        break;
      case MessageType::ADD:
        handle_add();
        break;
      case MessageType::SUB:
        handle_sub();
        break;
      case MessageType::MUL:
        handle_mul();
        break;
      case MessageType::DIV:
        handle_div();
        break;
      case MessageType::BYE:
        handle_bye();
        break;
      case MessageType::PUSH:
        handle_push(client_msg);
        break;
      case MessageType::SET:
        handle_set(client_msg);
        break;
      case MessageType::GET:
        handle_get(client_msg);
        break;
      default: throw OperationException("Please only enter standardized requests.");
    }
  }

  catch (CommException const& ex) {      manage_exception(ex, false); }
  catch (std::runtime_error const& ex) { manage_exception(ex, true); }
}


void ClientConnection::handle_create(Message client_msg) {

  m_server->lock();
  std::string table_name = client_msg.get_arg(0);

  // If the table's name is already in the server's map of tables
  if (m_server->find_table(table_name) != nullptr) {
    m_server->unlock();
    throw OperationException("A table with this name already exists.");
  }

  m_server->create_table(table_name);
  write_ok();

  m_server->unlock();
}


void ClientConnection::handle_login(bool* first_valid_message) {

  if(!first_valid_message) {
    throw OperationException("You are already logged in.");
  }

  *first_valid_message = false;
  write_ok();
}


void ClientConnection::handle_begin() {

  if (in_transaction) {
    throw FailedTransaction("Nested transactions are not supported.");
  }

  in_transaction = true;
  write_ok();
}


void ClientConnection::handle_commit() { 

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


void ClientConnection::handle_pop() {
  // Will throw OperationException if stack is empty
  stack.pop();
}


void ClientConnection::handle_top() {
  // Will throw OperationException if stack is empty
  std::string top_value = stack.get_top();

  // Create DATA Message
  Message data_msg(MessageType::DATA);
  data_msg.push_arg(top_value);

  // Encode and send data Message
  std::string encoded_data;
  encode(data_msg, encoded_data);
  rio_writen(m_client_fd, encoded_data.data(), encoded_data.size());
}


void ClientConnection::handle_add() {
  if (stack.get_size() < 2) {
    throw OperationException("There are not enough operands to add with.");
  }
  
  int right_operand;
  int left_operand;
  try {
    right_operand = std::stoi(stack.get_top());
    stack.pop();

    left_operand = std::stoi(stack.get_top());
    stack.pop();
  }
  catch (std::invalid_argument const& ex) {
    throw OperationException("Value on stack could not be converted to an integer.");
  }

  stack.push(std::to_string((left_operand + right_operand)));
  write_ok();
}


void ClientConnection::handle_sub() {
  if (stack.get_size() < 2) {
    throw OperationException("There are not enough operands to subtract with.");
  }
  
  int right_operand;
  int left_operand;
  try {
    right_operand = std::stoi(stack.get_top());
    stack.pop();

    left_operand = std::stoi(stack.get_top());
    stack.pop();
  }
  catch (std::invalid_argument const& ex) {
    throw OperationException("Value on stack could not be converted to an integer.");
  }

  stack.push(std::to_string((left_operand - right_operand)));
  write_ok();
}


void ClientConnection::handle_mul() {
  if (stack.get_size() < 2) {
    throw OperationException("There are not enough operands to multiply with.");
  }
  
  int right_operand;
  int left_operand;
  try {
    right_operand = std::stoi(stack.get_top());
    stack.pop();

    left_operand = std::stoi(stack.get_top());
    stack.pop();
  }
  catch (std::invalid_argument const& ex) {
    throw OperationException("Value on stack could not be converted to an integer.");
  }

  stack.push(std::to_string((left_operand * right_operand)));
  write_ok();
}


void ClientConnection::handle_div() {
  if (stack.get_size() < 2) {
    throw OperationException("There are not enough operands to divide with.");
  }
  
  int right_operand;
  int left_operand;
  try {
    right_operand = std::stoi(stack.get_top());
    stack.pop();

    left_operand = std::stoi(stack.get_top());
    stack.pop();
  }
  catch (std::invalid_argument const& ex) {
    throw OperationException("Value on stack could not be converted to an integer.");
  }

  stack.push(std::to_string((left_operand / right_operand)));
  write_ok();
}


void ClientConnection::handle_bye() {
  write_ok();
  free(this);
}


void ClientConnection::handle_push(Message client_msg) {
  stack.push(client_msg.get_arg(0));
  write_ok();
}


void ClientConnection::handle_set(Message client_msg) {

  std::string table_name = client_msg.get_arg(0);
  Table* table_obj = m_server->find_table(table_name);
  
  if (table_obj == nullptr) { throw OperationException("Could not find table."); }

  // During atomic operations where it is confirmed that the table exists
  if (!in_transaction) {
    table_obj->lock();
    try { set_table_value(client_msg, table_obj); }
    catch (OperationException const& ex) { throw OperationException(ex.what()); }

    table_obj->unlock();
    // During transactions
  } else {
    bool lock_successful = table_obj->trylock();

    if (lock_successful == false) { throw FailedTransaction("Could not monopolize access to table."); }
    try { set_table_value(client_msg, table_obj); }
    catch (OperationException const& ex) { throw OperationException(ex.what()); }
  }
  write_ok();
}


void ClientConnection::set_table_value(Message client_msg, Table* table_obj) {
  std::string key = client_msg.get_arg(1);

    // If the table does not have the key specified by the client, throw an exception
    if (!table_obj->has_key(key)) {
      table_obj->unlock();
      throw OperationException("Could not find key in specified table.");
    }
    // Otherwise, set the new value to the table (as a suggestion during transactions), and pop it
    if (!in_transaction) {
      table_obj->set(key, stack.get_top());
    } 
    else {
      table_obj->suggest_set(key, stack.get_top());
    }
    stack.pop(); 
}


void ClientConnection::handle_get(Message client_msg) {
  std::string table_name = client_msg.get_arg(0);
  Table* table_obj = m_server->find_table(table_name);
  
  if (table_obj == nullptr) { throw OperationException("Could not find table."); }
  
  // During atomic operations where it is confirmed that the table exists
  if (!in_transaction) {
    table_obj->lock();
    try { get_table_value(client_msg, table_obj); }
    catch (OperationException const& ex) { throw OperationException(ex.what()); }

    table_obj->unlock();
  // During transactions
  } else {
    bool lock_successful = table_obj->trylock();
    if (lock_successful == false) { throw FailedTransaction("Could not monopolize access to table."); }
    
    try { get_table_value(client_msg, table_obj); }
    catch (OperationException const& ex) { throw OperationException(ex.what()); }
  }
  write_ok();
}


void ClientConnection::get_table_value(Message client_msg, Table* table_obj) {
  std::string key = client_msg.get_arg(1);

  if (!table_obj->has_key(key)) {
      table_obj->unlock();
      throw OperationException("Could not find key in specified table.");
  } 
  else { stack.push(table_obj->get(key)); }
}


void ClientConnection::write_ok() {
  // Create OK Message
  Message ok_msg(MessageType::OK);

  // Encode and send Message
  std::string encoded_ok;
  encode(ok_msg, encoded_ok);
  rio_writen(m_client_fd, encoded_ok.data(), encoded_ok.size());
}

