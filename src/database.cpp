#include "../include/database.hpp"

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
      return "";
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