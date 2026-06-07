#include "env.h"
#include "admin.h"
#include "window.h"
#include <iostream>
#include <windows.h>

bool commandExists(Environment &env, std::string command) { return env.commands.find(command) != env.commands.end(); }
void executeCommand(Environment &env, const std::string &command)
{
    if (command == "hide")
    {
        HideToTray();
    }
    else
    {
        system(env.commands[command].c_str());
    }
}