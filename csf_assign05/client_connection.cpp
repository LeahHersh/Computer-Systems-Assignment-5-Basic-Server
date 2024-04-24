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
    }
  }
}

int ClientConnection::get_m_client_fd() 
{
  return m_client_fd;
}

// TODO: additional member functions
