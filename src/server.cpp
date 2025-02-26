#include "../include/server.hpp"
#include <iostream>
#include <memory>

VoltaxServer::VoltaxServer(int port) : acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    start_accept();
}

void VoltaxServer::start_accept()
{
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code &error)
                          {
      if (!error) handle_client(socket);
      start_accept(); });
}

void VoltaxServer::handle_client(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    auto buffer = std::make_shared<boost::asio::streambuf>();
    boost::asio::async_read_until(*socket, *buffer, "\n", [this, socket, buffer](const boost::system::error_code &error, size_t)
                                  {
      if (!error) {
          std::istream stream(buffer.get());
          std::string command, key, value;
          int ttl;
          stream >> command >> key;

          if (command == "SET") {
              stream >> value >> ttl;
              db.set(key, value, ttl);
              boost::asio::write(*socket, boost::asio::buffer("OK\n"));
          } else if (command == "GET") {
              boost::asio::write(*socket, boost::asio::buffer(db.get(key) + "\n"));
          } else if (command == "DEL") {
              db.del(key);
              boost::asio::write(*socket, boost::asio::buffer("OK\n"));
          } else if (command == "EXPIRE") {
              stream >> ttl;
              db.expire(key, ttl);
              boost::asio::write(*socket, boost::asio::buffer("OK\n"));
          } else if (command == "TTL") {
              boost::asio::write(*socket, boost::asio::buffer(std::to_string(db.ttl(key)) + "\n"));
          }
          handle_client(socket);
      } });
}

void VoltaxServer::run() { io_context.run(); }