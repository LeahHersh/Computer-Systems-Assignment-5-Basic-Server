#include <iostream>
#include <cassert>
#include <memory>
#include <iterator>
#include <pthread.h>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"

Server::Server() 
: server_fd(0)
{
  // TODO: implement
}

Server::~Server()
{
  // TODO: implement
}

void Server::listen( const std::string &port )
{
  int server_fd = open_listenfd(port.data());
  if (server_fd < 0) { throw CommException("Failed to create server socket"); }

}

void Server::server_loop()
{

  while (1) {
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
      log_error( "Could not accept a client" );
    }

    ClientConnection *client = new ClientConnection( this, client_fd );

    pthread_t thr_id;
    if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ) {
      log_error( "Could not create client thread" );
    }
    
  }

  // Note that your code to start a worker thread for a newly-connected
  // client might look something like this:
/*
  ClientConnection *client = new ClientConnection( this, client_fd );
  pthread_t thr_id;
  if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 )
    log_error( "Could not create client thread" );
*/
}


void *Server::client_worker( void *arg )
{
  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:
/*
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;
*/

std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();

  close(client->get_m_client_fd());
  free(arg);
  return nullptr;
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

// TODO: implement member functions
