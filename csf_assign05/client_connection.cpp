#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
  , in_transaction(false)
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  close(m_client_fd);
}

void ClientConnection::chat_with_client()
{
  // TODO: implement
}

int ClientConnection::get_m_client_fd() 
{
  return m_client_fd;
}

// TODO: additional member functions
