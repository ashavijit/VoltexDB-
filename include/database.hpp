#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>

class VoltaxDB
{
private:
  std::unordered_map<std::string, std::string> store;
  std::unordered_map<std::string, std::chrono::system_clock::time_point> expiry;
  std::unordered_map<std::string, std::list<std::string>> lists;
  std::mutex mtx;

public:
  void set(const std::string &key, const std::string &value, int ttl = -1);
  std::string get(const std::string &key);
  void del(const std::string &key);
  void expire(const std::string &key, int seconds);
  int ttl(const std::string &key);

  ///////////////////////////// LIST OPERSTIONS /////////////////////////////
  void lpush(const std::string &key, const std::string &value);
  void rpush(const std::string &key, const std::string &value);
  std::string lpop(const std::string &key);
  std::string rpop(const std::string &key);
  std::vector<std::string> lrange(const std::string &key, int start, int end);
  void lcycle(const std::string &key);
  void lremove(const std::string &key, const std::string &value);
  void lset(const std::string &key, int index, const std::string &value);
  void lunique(const std::string &key);
  void lshuffle(const std::string &key);

  // Tree operations (sorted keys retrieval)
  // std::vector<std::string> sorted_keys();

  ///////////////////////////// FILES  /////////////////////////////

  std::string snapshot_file = "snapshot.rdb";
  std::string aof_file = "appendonly.aof";
};

#endif // DATABASE_HPP