#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

class Environment
{
public:
  std::unordered_map<std::string, std::string> config;
  std::unordered_map<std::string, std::string> commands;

  bool loadConfig();
  bool loadCommands();
  std::vector<std::string> getSuggestions(std::string input);
};

bool Environment::loadConfig()
{
  std::string path = "./config/config.cnf";
  std::fstream configFile;

  configFile.open(path, std::ios::in);
  if (!configFile.good())
    return false;

  std::string line;
  while (std::getline(configFile, line))
  {
    std::size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos)
      return false;

    std::string key = line.substr(0, equalsPos);
    std::string value = line.substr(equalsPos + 1);
    config[key] = value;
  }

  configFile.close();
  return true;
}

bool Environment::loadCommands()
{
  std::string path = "./config/commands.cnf";
  std::fstream commandsFile;

  commandsFile.open(path, std::ios::in);
  if (!commandsFile.good())
    return false;

  std::string line;
  while (std::getline(commandsFile, line))
  {
    std::size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos)
      return false;

    std::string key = line.substr(0, equalsPos);
    std::string value = line.substr(equalsPos + 1);
    commands[key] = value;
  }

  commandsFile.close();
  return true;
}

std::vector<std::string> Environment::getSuggestions(std::string input)
{
  std::vector<std::string> suggestions;

  for (const auto &pair : commands)
  {
    const std::string &commandName = pair.first;
    if (commandName.rfind(input, 0) == 0)
    {
      suggestions.push_back(commandName);
    }
  }
  std::sort(suggestions.begin(), suggestions.end());
  return suggestions;
}