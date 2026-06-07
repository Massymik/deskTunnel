#pragma once
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <sstream>

void clearFile() { std::ofstream ofs("server.log", std::ios::out | std::ios::trunc); }

void log(const std::string &message)
{
  std::fstream logFile("server.log", std::ios::app | std::ios::in);
  if (logFile.is_open())
  {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string line;
    std::stringstream date;
    date << std::put_time(std::localtime(&now), "%Y-%m-%d");

    logFile.seekg(0);
    std::getline(logFile, line);
    if (line.length() >= 10)
    {
      line = line.substr(0, 10);
      logFile.seekp(0, std::ios::end);
    }

    if (line != date.str())
    {
      logFile.close();
      clearFile();
      logFile.open("server.log", std::ios::app | std::ios::in);
    }
    logFile << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
  }
}