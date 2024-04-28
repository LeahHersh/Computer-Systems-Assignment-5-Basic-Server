#include <iostream>
#include "server.h"

int main(int argc, char **argv)
{
  std::cerr << "Hit start\n";
  if ( argc != 2 ) {
    std::cerr << "Usage: ./server <port>\n";
    return 1;
  }
  std::cerr << "Passed args\n";
  Server server;
  std::cerr << "Point 1\n";

  try {
    std::cerr << "Hit constructor\n";
    server.listen( argv[1] );
    std::cerr << "Hit listen\n";
    server.server_loop();
    std::cerr << "Hit loop\n";
  } catch ( std::runtime_error &ex ) {
    server.log_error( "Fatal error starting server" );
    return 1;
  }

  return 0;
}
