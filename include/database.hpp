#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <unordered_map>
#include <mutex>
#include <chrono>

class VoltaxDB
{
private:
  std::unordered_map<std::string, std::string> store;
  std::unordered_map<std::string, std::chrono::system_clock::time_point> expiry;
  std::mutex mtx;

public:
  void set(const std::string &key, const std::string &value, int ttl = -1);
  std::string get(const std::string &key);
  void del(const std::string &key);
  void expire(const std::string &key, int seconds);
  int ttl(const std::string &key);
};

#endif // DATABASE_HPP