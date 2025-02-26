#include "../include/server.hpp"
#include <iostream>

int main()
{
  VoltaxServer server(8080);
  server.run();
  printf("Server is running on port 8080\n");
  return 0;
}