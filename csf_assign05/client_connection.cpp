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
  , in_transaction(false)
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  for (auto it = locked_tables.begin(); it != locked_tables.end(); it++) {
    (*it)->unlock();
  }

  close(m_client_fd);
}

void ClientConnection::chat_with_client()
{
  bool input_left = true;
  Message response_msg;
  char buf[response_msg.MAX_ENCODED_LEN];

  while (input_left) {
    ssize_t n = Rio_readlineb(&m_fdbuf, buf, sizeof(m_fdbuf));
    // If nothing is read from the client
    if (n <= 0) {
      input_left = false;
    }

    else {
      decode(buf, response_msg);
      call_response_function(response_msg);
    }
  }
}

void ClientConnection::call_response_function(Message response_msg) {
  MessageType response_type = response_msg.get_message_type();
  
  // Choose which helper function to call
  switch (response_type) {

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
  case MessageType::CREATE:
    handle_create(response_msg);
    break;
  case MessageType::LOGIN:
    handle_login(response_msg);
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
  default: throw InvalidMessage("Client message did not have a valid request type.");
  }
}


int ClientConnection::get_m_client_fd() 
{
  return m_client_fd;
}

// TODO: additional member functions
