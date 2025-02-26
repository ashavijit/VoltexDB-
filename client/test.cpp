// #include "../include/database.hpp"
// #include "database.hpp"
// #include <optional>
// #include <algorithm>
// #include <random>

// /**
//  * @brief Stores a key-value pair with an optional TTL.
//  *
//  * @param key The key to store.
//  * @param value The value to associate with the key.
//  * @param ttl Time-to-live in seconds (optional).
//  */
// void VoltaxDB::set(const std::string &key, const std::string &value, int ttl) noexcept
// {
//   std::scoped_lock lock(mtx);
//   store[key] = value;
//   if (ttl > 0)
//   {
//     expiry[key] = std::chrono::system_clock::now() + std::chrono::seconds(ttl);
//   }
// }

// /**
//  * @brief Retrieves the value of a key.
//  *
//  * @param key The key to fetch.
//  * @return std::optional<std::string> The value if found, otherwise std::nullopt.
//  */
// std::optional<std::string> VoltaxDB::get(const std::string &key) noexcept
// {
//   std::scoped_lock lock(mtx);
//   auto it = store.find(key);
//   if (it == store.end())
//     return std::nullopt;

//   auto exp_it = expiry.find(key);
//   if (exp_it != expiry.end() && std::chrono::system_clock::now() > exp_it->second)
//   {
//     store.erase(it);
//     expiry.erase(exp_it);
//     return std::nullopt;
//   }
//   return it->second;
// }

// /**
//  * @brief Deletes a key-value pair.
//  *
//  * @param key The key to delete.
//  */
// void VoltaxDB::del(const std::string &key) noexcept
// {
//   std::scoped_lock lock(mtx);
//   store.erase(key);
//   expiry.erase(key);
// }

// /**
//  * @brief Sets an expiration time on a key.
//  *
//  * @param key The key to modify.
//  * @param seconds The TTL in seconds.
//  */
// void VoltaxDB::expire(const std::string &key, int seconds) noexcept
// {
//   std::scoped_lock lock(mtx);
//   if (store.contains(key))
//   {
//     expiry[key] = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
//   }
// }

// /**
//  * @brief Gets the TTL of a key.
//  *
//  * @param key The key to check.
//  * @return int TTL in seconds, -1 if no expiry, -2 if key does not exist.
//  */
// int VoltaxDB::ttl(const std::string &key) noexcept
// {
//   std::scoped_lock lock(mtx);
//   if (!store.contains(key))
//     return -2;
//   auto it = expiry.find(key);
//   if (it == expiry.end())
//     return -1;

//   auto now = std::chrono::system_clock::now();
//   if (now > it->second)
//   {
//     store.erase(key);
//     expiry.erase(it);
//     return -2;
//   }
//   return std::chrono::duration_cast<std::chrono::seconds>(it->second - now).count();
// }

// //// ============================= LIST OPERATIONS ============================= ////

// /**
//  * @brief Pushes an element to the front of the list.
//  *
//  * @param key The key of the list.
//  * @param value The value to push.
//  */
// void VoltaxDB::lpush(const std::string &key, const std::string &value) noexcept
// {
//   std::scoped_lock lock(mtx);
//   list_store[key].push_front(value);
// }

// /**
//  * @brief Pushes an element to the back of the list.
//  *
//  * @param key The key of the list.
//  * @param value The value to push.
//  */
// void VoltaxDB::rpush(const std::string &key, const std::string &value) noexcept
// {
//   std::scoped_lock lock(mtx);
//   list_store[key].push_back(value);
// }

// /**
//  * @brief Pops an element from the front of the list.
//  *
//  * @param key The key of the list.
//  * @return std::optional<std::string> The popped value if exists, otherwise std::nullopt.
//  */
// std::optional<std::string> VoltaxDB::lpop(const std::string &key) noexcept
// {
//   std::scoped_lock lock(mtx);
//   auto it = list_store.find(key);
//   if (it == list_store.end() || it->second.empty())
//     return std::nullopt;

//   std::string value = std::move(it->second.front());
//   it->second.pop_front();
//   return value;
// }

// /**
//  * @brief Pops an element from the back of the list.
//  *
//  * @param key The key of the list.
//  * @return std::optional<std::string> The popped value if exists, otherwise std::nullopt.
//  */
// std::optional<std::string> VoltaxDB::rpop(const std::string &key) noexcept
// {
//   std::scoped_lock lock(mtx);
//   auto it = list_store.find(key);
//   if (it == list_store.end() || it->second.empty())
//     return std::nullopt;

//   std::string value = std::move(it->second.back());
//   it->second.pop_back();
//   return value;
// }

// /**
//  * @brief Gets a range of values from a list.
//  *
//  * @param key The key of the list.
//  * @param start The starting index.
//  * @param end The ending index.
//  * @return std::vector<std::string> The list elements in the range.
//  */
// std::vector<std::string> VoltaxDB::lrange(const std::string &key, int start, int end) noexcept
// {
//   std::scoped_lock lock(mtx);
//   auto it = list_store.find(key);
//   if (it == list_store.end())
//     return {};

//   auto &lst = it->second;
//   if (start < 0)
//     start = std::max(0, static_cast<int>(lst.size()) + start);
//   if (end < 0)
//     end = std::max(0, static_cast<int>(lst.size()) + end);

//   if (start >= end || start >= static_cast<int>(lst.size()))
//     return {};

//   std::vector<std::string> result;
//   auto list_start = std::next(lst.begin(), start);
//   auto list_end = std::next(lst.begin(), std::min(end, static_cast<int>(lst.size())));
//   result.assign(list_start, list_end);

//   return result;
// }

// /**
//  * @brief Cycles the first element of the list to the end.
//  */
// void VoltaxDB::lcycle(const std::string &key) noexcept
// {
//   std::scoped_lock lock(mtx);
//   auto it = list_store.find(key);
//   if (it == list_store.end() || it->second.empty())
//     return;

//   it->second.push_back(std::move(it->second.front()));
//   it->second.pop_front();
// }

// /**
//  * @brief Removes all occurrences of a value in the list.
//  */
// void VoltaxDB::lremove(const std::string &key, const std::string &value) noexcept
// {
//   std::scoped_lock lock(mtx);
//   auto it = list_store.find(key);
//   if (it != list_store.end())
//   {
//     it->second.remove(value);
//   }
// }

// /**
//  * @brief Shuffles the elements in the list.
//  */
// void VoltaxDB::lshuffle(const std::string &key) noexcept
// {
//   std::scoped_lock lock(mtx);
//   auto it = list_store.find(key);
//   if (it == list_store.end())
//     return;

//   std::random_device rd;
//   std::mt19937 g(rd());
//   std::vector<std::string> temp(it->second.begin(), it->second.end());
//   std::shuffle(temp.begin(), temp.end(), g);
//   it->second.assign(temp.begin(), temp.end());
// }

// /**
//  * @brief Returns a sorted vector of stored keys.
//  */
// std::vector<std::string> VoltaxDB::sorted_keys() noexcept
// {
//   std::scoped_lock lock(mtx);
//   std::vector<std::string> keys;
//   keys.reserve(store.size());
//   for (const auto &[key, _] : store)
//   {
//     keys.push_back(key);
//   }
//   std::sort(keys.begin(), keys.end());
//   return keys;
// }
