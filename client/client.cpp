#include <iostream>
#include <boost/asio.hpp>

int main()
{
  using namespace boost::asio;
  io_context io;
  ip::tcp::socket socket(io);
  socket.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8080));

  auto send_command = [&](const std::string &cmd)
  {
    write(socket, buffer(cmd + "\n"));
    char reply[1024] = {0};
    socket.read_some(buffer(reply));
    std::cout << "Response: " << reply;
  };

  send_command("SET foo bar 10");
  send_command("GET foo");
  send_command("TTL foo");
  send_command("DEL foo");
  send_command("GET foo");
  /// list

  send_command("LPUSH mylist 1");
  send_command("LPUSH mylist 2");
  send_command("LPUSH mylist 3");
  send_command("LRANGE mylist 0 2");

  return 0;
}
