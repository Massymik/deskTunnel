#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"
#define GRAY "\033[90m"
#define LINE " ─────────────────────────────────────────────────────────" RESET

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include "./src/getContents.h"
#include "./src/window.h"
#include "./src/env.h"
#include "./src/createLog.h"
#include "./src/mongoose.h"
#include "./src/textTools.h"
#include "./src/webSocketHandler.h"
#include "./src/admin.h"
#include "./database/db.h"
#include "./database/auth.h"

int main()
{
  InitializeTray();
  if (!isRunningAsAdmin())
  {
    if (restartAsAdmin())
    {
      FreeConsole();
      return 0;
    }
    return exitError("Administrator permission is required");
  }

  std::cout << "\033[?25l";
  SetConsoleOutputCP(65001);
  printLogo();

  animationRunning = true;
  std::thread animThread(dotsAnimation, BLUE "[INIT]" RESET " Creating database");

  if (!createDB())
  {
    animationRunning = false;
    animThread.join();
    log("[ERROR] (main) while attempting to create a DB");
    return exitError("While attempting to create a DB");
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  animationRunning = false;
  animThread.join();
  printf(GREEN "\n  [OK]" RESET " Database ready\n");

  animationRunning = true;
  std::thread animThread2(dotsAnimation, BLUE "[INIT]" RESET " Loading config");
  Environment env;

  if (!env.loadConfig())
  {
    animationRunning = false;
    animThread2.join();
    log("[ERROR] (main) while loading config");
    return exitError("While loading config");
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  animationRunning = false;
  animThread2.join();
  printf(GREEN "\n  [OK]" RESET " Config loaded\n");

  animationRunning = true;
  std::thread animThread3(dotsAnimation, BLUE "[INIT]" RESET " Loading commands");
  if (!env.loadCommands())
  {
    animationRunning = false;
    animThread3.join();
    log("[ERROR] (main) while loading commands");
    return exitError("While loading commands");
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  animationRunning = false;
  animThread3.join();
  printf(GREEN "\n  [OK]" RESET " Commands loaded\n");

  if (env.config["homePath"].empty())
  {
    log("[ERROR] (main) homePath is empty");
    return exitError("HomePath is empty");
  }

  std::string homePath = env.config["homePath"];
  std::thread watcherThread([homePath]()
                            {
    while (true) {
      awaitChanges(homePath);
      {
        std::lock_guard<std::mutex> lock(changesMutex);
        shouldSendUpdate = true;
      }
    } });

  std::thread commandInput([&env]()
                           { handleTyping(env); });

  mg_log_set(MG_LL_ERROR);
  mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_connection *conn = mg_http_listen(&mgr, "http://localhost:9002", websocket_handler, &homePath);

  if (conn == nullptr)
  {
    watcherThread.detach();
    log("[ERROR] (main) while starting WebSocket");
    return exitError("While starting WebSocket");
  }

  printf(CYAN "\n  [INFO]" RESET " WebSocket listening on ws://localhost:9002\n");
  printf(CYAN "  [INFO]" RESET " Watching: %s", homePath.c_str());
  printf("\n\n");
  std::thread animThread4(uiAnimation);
  animThread4.detach();

  while (true)
  {
    mg_mgr_poll(&mgr, 100);
    bool sendUpdate = false;

    {
      std::lock_guard<std::mutex> lock(changesMutex);
      if (shouldSendUpdate)
      {
        shouldSendUpdate = false;
        sendUpdate = true;
      }
    }

    if (sendUpdate)
    {
      std::filesystem::path homePath1 = homePath;
      sendContentsToAll(homePath1);
    }
  }
  mg_mgr_free(&mgr);
  watcherThread.join();
  std::cout << "\033[?25h";

  return 0;
}