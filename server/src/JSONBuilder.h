#pragma once
#include <vector>
#include <string>

static std::string escapeJSON(const std::string &text)
{
  std::string result;

  for (char c : text)
  {
    switch (c)
    {
    case '"':
      result += "\\\"";
      break;
    case '\\':
      result += "\\\\";
      break;
    case '\n':
      result += "\\n";
      break;
    case '\r':
      result += "\\r";
      break;
    case '\t':
      result += "\\t";
      break;
    default:
      result += c;
      break;
    }
  }

  return result;
}

static std::string createDirectoryJSON(const std::filesystem::path &directory)
{
  std::string json = "[";
  bool first = true;

  std::filesystem::directory_options options =
      std::filesystem::directory_options::skip_permission_denied;

  for (const auto &entry : std::filesystem::directory_iterator(directory, options))
  {
    if (!first)
    {
      json += ",";
    }

    first = false;

    std::string name = entry.path().filename().string();

    if (entry.is_directory())
    {
      json += "{";
      json += "\"type\":\"folder\",";
      json += "\"name\":\"" + escapeJSON(name) + "\",";
      json += "\"children\":";
      json += createDirectoryJSON(entry.path());
      json += "}";
    }
    else
    {
      json += "{";
      json += "\"type\":\"file\",";
      json += "\"name\":\"" + escapeJSON(name) + "\"";
      json += "}";
    }
  }

  json += "]";
  return json;
}