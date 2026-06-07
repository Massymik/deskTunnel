#pragma once
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <conio.h>
#include "env.h"
#include "commands.h"

inline void printLogo() {
  std::cout << R"(     ____                      _          _                      
    |  _ \ ___ _ __ ___   ___ | |_ ___   / \   ___ ___ ___  ___ 
    | |_) / _ \ '_ ` _ \ / _ \| __/ _ \ / _ \ / __/ __/ _ \/ __|
    |  _ <  __/ | | | | | (_) | ||  __// ___ \ (_| (_|  __/\__ \
    |_| \_\___|_| |_| |_|\___/ \__\___/_/   \_\___\___\___||___/
)" << std::endl;
}

std::atomic<bool> animationRunning = true;
void dotsAnimation(std::string text) {
  const char *frames[] = {
      "[          ]", "[░         ]", "[▒░        ]", "[▓▒░       ]", "[█▓▒░      ]", "[██▓▒░     ]", "[░██▓▒░    ]",
      "[ ░██▓▒░   ]", "[  ░██▓▒░  ]", "[   ░██▓▒░ ]", "[    ░██▓▒░]", "[     ░██▓▒]", "[      ░██▓]", "[       ░██]",
      "[        ░█]", "[         ░]", "[          ]", "[         ░]", "[        ░█]", "[       ██░]", "[      ▓██░]",
      "[     ▒▓██░]", "[    ░▒▓██░]", "[   ░▒▓██░ ]", "[  ░▒▓██░  ]", "[ ░▒▓██░   ]", "[░▒▓██░    ]", "[▒▓██░     ]",
      "[▓██░      ]", "[██░       ]", "[█░        ]", "[░         ]"};
  int i = 0;
  int frameCount = sizeof(frames) / sizeof(frames[0]);
  int animLength = 750;

  while (animationRunning) {
    std::cout << "\r  " << text << " " << frames[i];
    i = (i + 1) % frameCount;
    std::this_thread::sleep_for(std::chrono::milliseconds(animLength / frameCount));
  }
  std::cout << "\r  " << text << "                  ";
}

std::string commandLineString = "";
std::string suggestion = "";
void appendCommandLine(char c) { commandLineString += c; }

std::string getCommandLineString() { return commandLineString; }
void setCommandLineString(std::string command) { commandLineString = command; }
std::string getSuggestion() { return suggestion; }
void setSuggestion(std::string command) { suggestion = command; }

void removeCommandLineChar() {
  if (!commandLineString.empty()) {
    commandLineString.pop_back();
  }
}

void uiAnimation() {
  const char *frames[] = {"   .   ", "   o   ", "  (o)  ", " ((o)) ", "(((o)))", " ((o)) ", "  (o)  ", "   o   "};
  int frameCount = sizeof(frames) / sizeof(frames[0]);
  int i = 0;
  int serverTick = 0;
  int promptTick = 0;
  bool showPrompt = true;

  while (true) {
    printf("\r  Server running %s        ", frames[i]);
    printf("\n\n");
    printf("\r  " MAGENTA "[SERVER]" RESET CYAN "[INFO]" RESET " type in command.        \n");

    printf("\r\033[2K");
    if (showPrompt) {
      printf("  > %s" GRAY "%s" RESET, commandLineString.c_str(), suggestion.c_str());
    } else {
      printf("    %s" GRAY "%s" RESET, commandLineString.c_str(), suggestion.c_str());
    }

    serverTick++;
    promptTick++;
    if (serverTick >= 6) {
      i = (i + 1) % frameCount;
      serverTick = 0;
    }
    if (promptTick >= 8) {
      showPrompt = !showPrompt;
      promptTick = 0;
    }
    printf("\033[3A");
    fflush(stdout);

    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
}

int exitError(std::string message) {
  std::cout << "\033[?25h" << std::flush;
  printf(RED "\n  [ERROR]" RESET " %s\n", message.c_str());
  return -1;
}

void handleTyping(Environment &env) {
  while (true) {
    if (_kbhit()) {
      char c = _getch();
      if (c == '\t') {
        // tab
        std::string userInput = getCommandLineString();
        setCommandLineString(userInput + getSuggestion());
        setSuggestion("");
      } else if (c == 27) {
        // esc
        setCommandLineString("");
        setSuggestion("");
      } else if (c == 8) {
        // backspace
        removeCommandLineChar();
        std::string userInput = getCommandLineString();
        std::vector<std::string> suggestions = env.getSuggestions(userInput);
        if (userInput.empty() || suggestions.empty()) {
          setSuggestion("");
          continue;
        }

        setSuggestion(suggestions[0].substr(userInput.length()));
      } else if (c == 13) {
        std::string userInput = getCommandLineString();
        if (!commandExists(env, userInput)) {
          setCommandLineString(RED "[ERROR]" RESET "(command) cannot find command: " + userInput);
          continue;
        }
        executeCommand(env, userInput);
        setCommandLineString("");
      } else if (c == 0 || c == 224) {
        int special = _getch();

        switch (special) {
          case 72:
            // up
            break;
          case 80:
            // down
            break;
          case 75:
            // left
            break;
          case 77:
            // right
            break;
          case 83:
            // delete
            break;
        }
      } else if (c >= 97 && c <= 122 || c == 32) {
        appendCommandLine(c);
        std::string userInput = getCommandLineString();
        std::vector<std::string> suggestions = env.getSuggestions(userInput);
        if (suggestions.empty()) {
          setSuggestion("");
          continue;
        }
        setSuggestion(suggestions[0].substr(userInput.length()));
      }
    }
  }
}