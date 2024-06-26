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

  pthread_mutex_t mutex;

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

  void lock();

  void unlock();

  std::unordered_map<std::string, Table*> get_table_map() { return table_names; }

  void create_table( const std::string &name );

  /* Check if a table is in the server's table map. Return the table if it is, and 
  return nullptr otherwise. */
  Table* find_table( const std::string &name );
};


#endif // SERVER_H
