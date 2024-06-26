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
  // Mutex is used to lock a server while tables are being created
  pthread_mutex_init(&mutex, NULL);
}

Server::~Server()
{
  close(server_fd);
  pthread_mutex_destroy(&mutex);
}

void Server::listen( const std::string &port )
{
  server_fd = open_listenfd(port.data());
  if (server_fd < 0) { throw CommException("Failed to create server socket"); }

}

void Server::server_loop() {

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
}


void *Server::client_worker( void *arg )
{
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );

  client->chat_with_client();
  return nullptr;
}


void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}


void Server::lock()
{ 
  pthread_mutex_lock(&mutex);
  mutex_is_locked = true;
}


void Server::unlock() {
  mutex_is_locked = false;
  pthread_mutex_unlock(&mutex);
}


void Server::create_table( const std::string &name ) {
  Table* new_table = new Table(name);

  table_names[name] = new_table;
}


Table* Server::find_table( const std::string &name ) {

  // If the table name is in the map, return its corresponding table
  if (table_names.find(name) != table_names.end()) {
    return table_names[name];
  }

  return nullptr;
}
