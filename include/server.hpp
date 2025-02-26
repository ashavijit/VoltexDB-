#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include "database.hpp"

class VoltaxServer
{
private:
  boost::asio::io_context io_context;
  boost::asio::ip::tcp::acceptor acceptor;
  VoltaxDB db;
  void start_accept();
  void handle_client(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

public:
  VoltaxServer(int port);
  void run();
  // void handle_client(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
};
#endif // SERVER_HPP