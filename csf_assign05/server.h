#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>
#include <string>
#include <pthread.h>
#include "table.h"
#include "client_connection.h"

class Server {
private:
  int server_fd;
  std::unordered_map<std::string, Table*> table_names;

  pthread_mutex_t* mutex;

  bool mutex_is_locked;

  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  void listen( const std::string &port );

  void server_loop();

  static void *client_worker( void *arg );

  void log_error( const std::string &what );

  // TODO: add member functions

  // Some suggested member functions:
  void create_table( const std::string &name );

  Table *find_table( const std::string &name );
};


#endif // SERVER_H
