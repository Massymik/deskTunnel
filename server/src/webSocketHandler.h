#pragma once
#include <iostream>
#include <filesystem>
#include <set>
#include <map>
#include <mutex>

#include "mongoose.h"
#include "./textTools.h"
#include "./getContents.h"
#include "./JSONBuilder.h"
#include "./userOperations.h"

static std::mutex changesMutex;
static bool shouldSendUpdate = false;

static std::map<std::string, std::string> users = {{"ABC123", "michal"}, {"XYZ789", "user2"}};
static std::set<mg_connection *> clients;
static std::map<mg_connection *, std::string> connectionUsers;

static std::string get_user_by_key(const std::string &key)
{
  auto it = users.find(key);
  if (it == users.end())
  {
    return "";
  }
  return it->second;
}

static void broadcast(mg_connection *sender, const std::string &msg)
{
  for (auto *client : clients)
  {
    if (client != sender)
    {
      mg_ws_send(client, msg.c_str(), msg.size(), WEBSOCKET_OP_TEXT);
    }
  }
}

void websocket_handler(mg_connection *c, int ev, void *ev_data)
{
  void *fn_data = c->fn_data;
  if (ev == MG_EV_HTTP_MSG)
  {
    mg_http_message *hm = (mg_http_message *)ev_data;

    if (mg_match(hm->uri, mg_str("/ws"), nullptr))
    {
      mg_ws_upgrade(c, hm, nullptr);
      return;
    }

    mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Use ws://localhost:9002/ws\n");
  }

  if (ev == MG_EV_WS_OPEN)
  {
    clients.insert(c);
  }

  if (ev == MG_EV_CLOSE)
  {
    clients.erase(c);
  }

  if (ev == MG_EV_WS_MSG)
  {
    mg_ws_message *wm = (mg_ws_message *)ev_data;
    std::string msg(wm->data.buf, wm->data.len);

    if (msg == "start")
    {
      std::string *homePath = static_cast<std::string *>(fn_data);
      std::filesystem::path directory = *homePath;

      std::string JSON = createDirectoryJSON(directory);

      mg_ws_send(c, JSON.c_str(), JSON.size(), WEBSOCKET_OP_TEXT);
    }
    else
    {
      char *operation = mg_json_get_str(mg_str_n(msg.data(), msg.size()), "$.operation");
      char *path = mg_json_get_str(mg_str_n(msg.data(), msg.size()), "$.path");
      std::string operationStr = operation;
      std::filesystem::path relativePath = path;
      if (operation != nullptr && path != nullptr)
      {
        if (operationStr == "getFileContents")
        {
          std::string contents = getFileContents(relativePath);
          std::string filename = relativePath.filename().string();

          std::string response = "{";
          response += "\"type\":\"fileContents\",";
          response += "\"path\":\"" + escapeJSON(relativePath.generic_string()) + "\",";
          response += "\"fileName\":\"" + escapeJSON(filename) + "\",";
          response += "\"contents\":\"" + base64Encode(contents) + "\"";
          response += "}";

          mg_ws_send(c, response.c_str(), response.size(), WEBSOCKET_OP_TEXT);
        }
      }
    }
  }
}

static void sendContentsToAll(const std::filesystem::path &directory)
{
  std::string JSON = createDirectoryJSON(directory);

  for (auto *client : clients)
  {
    mg_ws_send(client, JSON.c_str(), JSON.size(), WEBSOCKET_OP_TEXT);
  }
}