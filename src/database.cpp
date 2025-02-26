#include "../include/database.hpp"
#include "database.hpp"

void VoltaxDB::set(const std::string &key, const std::string &value, int ttl)
{
  std::lock_guard<std::mutex> lock(mtx);
  store[key] = value;
  if (ttl > 0)
  {
    expiry[key] = std::chrono::system_clock::now() + std::chrono::seconds(ttl);
  }
}

std::string VoltaxDB::get(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  if (store.find(key) == store.end())
  {
    return "";
  }
  if (expiry.find(key) != expiry.end())
  {
    if (std::chrono::system_clock::now() > expiry[key])
    {
      store.erase(key);
      expiry.erase(key);
      return "(nil)";
    }
  }
  return store[key];
}

void VoltaxDB::del(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  store.erase(key);
  expiry.erase(key);
}

void VoltaxDB::expire(const std::string &key, int seconds)
{
  std::lock_guard<std::mutex> lock(mtx);
  if (store.find(key) == store.end())
  {
    return;
  }
  expiry[key] = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
}

int VoltaxDB::ttl(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  if (store.find(key) == store.end())
  {
    return -2;
  }
  if (expiry.find(key) == expiry.end())
  {
    return -1;
  }
  auto now = std::chrono::system_clock::now();
  if (now > expiry[key])
  {
    store.erase(key);
    expiry.erase(key);
    return -2;
  }
  return std::chrono::duration_cast<std::chrono::seconds>(expiry[key] - now).count();
}

//// list operations

// List operations
void VoltaxDB::lpush(const std::string &key, const std::string &value)
{
  std::lock_guard<std::mutex> lock(mtx);
  lists[key].push_front(value);
}

void VoltaxDB::rpush(const std::string &key, const std::string &value)
{
  std::lock_guard<std::mutex> lock(mtx);
  lists[key].push_back(value);
}

std::string VoltaxDB::lpop(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  if (lists[key].empty())
    return "(nil)";
  std::string value = lists[key].front();
  lists[key].pop_front();
  return value;
}

std::string VoltaxDB::rpop(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  if (lists[key].empty())
    return "(nil)";
  std::string value = lists[key].back();
  lists[key].pop_back();
  return value;
}

std::vector<std::string> VoltaxDB::lrange(const std::string &key, int start, int end)
{
  std::lock_guard<std::mutex> lock(mtx);
  if (!lists.count(key))
    return {};
  auto &lst = lists[key];
  auto it_start = std::next(lst.begin(), std::max(0, start));
  auto it_end = std::next(lst.begin(), std::min((int)lst.size(), end + 1));
  return std::vector<std::string>(it_start, it_end);
}

void VoltaxDB::lcycle(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  if (!lists[key].empty())
  {
    lists[key].push_back(lists[key].front());
    lists[key].pop_front();
  }
}

void VoltaxDB::lremove(const std::string &key, const std::string &value)
{
  std::lock_guard<std::mutex> lock(mtx);
  lists[key].remove(value);
}

void VoltaxDB::lset(const std::string &key, int index, const std::string &value)
{
  std::lock_guard<std::mutex> lock(mtx);
  auto it = std::next(lists[key].begin(), index);
  if (it != lists[key].end())
    *it = value;
}

void VoltaxDB::lunique(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  lists[key].sort();
  lists[key].unique();
}

void VoltaxDB::lshuffle(const std::string &key)
{
  std::lock_guard<std::mutex> lock(mtx);
  std::vector<std::string> temp(lists[key].begin(), lists[key].end());
  std::shuffle(temp.begin(), temp.end(), std::mt19937{std::random_device{}()});
  lists[key] = std::list<std::string>(temp.begin(), temp.end());
}
