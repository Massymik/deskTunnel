#pragma once
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

void startChrome() {
  std::string htmlPath = "./src/html/index.html";
  std::string pngPath = "./src/res/html.png";
  std::string command = "start \"\" "
                        "\"C:/Program Files/Google/Chrome/Application/chrome.exe\" "
                        "--headless=new "
                        "--remote-debugging-port=9222 "
                        "--disable-gpu "
                        "--user-data-dir=\"D:/chrome-cdp-profile\"";

  std::system(command.c_str());
}

void convertHtmlToImg() {}