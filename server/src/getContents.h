#pragma once
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>

using FileSnapshot = std::unordered_map<std::string, std::filesystem::file_time_type>;

struct Item
{
  std::string name;
  bool isDirectory;
};

// ----=============----
std::vector<Item> getContents(const std::filesystem::path &directory); // 1
void awaitChanges(std::filesystem::path directory);                    // 2
FileSnapshot scan_directory(const std::filesystem::path &directory);   // 3
// ----=============----

// ----===== 1 =====----i
inline std::vector<Item> getContents(const std::filesystem::path &directory)
{
  std::vector<Item> contents;

  for (const auto &entry : std::filesystem::directory_iterator(directory))
  {
    contents.push_back({entry.path().filename().string(), entry.is_directory()});
  }

  return contents;
}

// ----===== 2 =====----
inline void awaitChanges(std::filesystem::path directory)
{
  auto previous = scan_directory(directory);

  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto current = scan_directory(directory);

    for (const auto &[path, time] : current)
    {
      if (previous.find(path) == previous.end())
      {
        return;
      }
      else if (previous[path] != time)
      {
        return;
      }
    }

    for (const auto &[path, time] : previous)
    {
      if (current.find(path) == current.end())
      {
        return;
      }
    }

    previous = current;
  }
}

// ----===== 3 =====----
inline FileSnapshot scan_directory(const std::filesystem::path &directory)
{
  FileSnapshot snapshot;

  std::filesystem::directory_options options = std::filesystem::directory_options::skip_permission_denied;

  for (const auto &entry : std::filesystem::recursive_directory_iterator(directory, options))
  {
    try
    {
      if (entry.is_regular_file() || entry.is_directory())
      {
        snapshot[entry.path().string()] = std::filesystem::last_write_time(entry);
      }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
    }
  }

  return snapshot;
}