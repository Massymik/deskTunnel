#pragma once
#include <windows.h>
#include <string>
#include <shellapi.h>

bool isRunningAsAdmin() {
  BOOL isAdmin = FALSE;
  PSID adminGroup = nullptr;

  SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

  if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                               &adminGroup)) {
    CheckTokenMembership(nullptr, adminGroup, &isAdmin);
    FreeSid(adminGroup);
  }

  return isAdmin == TRUE;
}

bool restartAsAdmin() {
  char exePath[MAX_PATH];

  if (!GetModuleFileNameA(nullptr, exePath, MAX_PATH)) {
    return false;
  }

  std::filesystem::path exe = exePath;
  std::string exeDir = exe.parent_path().string();

  std::string wtArgs = "-w RemoteAccess new-tab "
                       "--title \"RemoteAccess\" "
                       "-d \"" +
                       exeDir +
                       "\" "
                       "cmd /k \"\\\"" +
                       std::string(exePath) + "\\\"\"";

  HINSTANCE result = ShellExecuteA(nullptr, "runas", "wt.exe", wtArgs.c_str(), exeDir.c_str(), SW_SHOWNORMAL);

  return reinterpret_cast<intptr_t>(result) > 32;
}