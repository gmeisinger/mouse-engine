/**
 * @file Logger.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Logger.h"
using namespace std;

namespace mouse {

Logger *Logger::singleton = nullptr;

Logger::Logger(const string &filename) {
  logFile.open(filename, ios::app);
  if (!logFile.is_open()) {
    cerr << "Error opening log file." << endl;
    cerr << filename << endl;
  }
}
Logger::~Logger() { logFile.close(); }

void Logger::log(const std::string &msg) {
  cerr << msg << endl;
  // Get current timestamp
  time_t now = time(0);
  tm *timeinfo = localtime(&now);
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

  // Create log entry
  ostringstream logEntry;
  logEntry << "[" << timestamp << "]: " << msg << endl;

  // Output to log file
  if (logFile.is_open()) {
    logFile << logEntry.str();
    logFile.flush(); // Ensure immediate write to file
  }
}

int Logger::l_log(lua_State *L) {
  // first arg is the string
  const char *str = (const char *)luaL_checkstring(L, 1);
  Logger::singleton->log(str);
  return 0;
}

std::vector<luaL_Reg> Logger::l_funcs = {{"log", Logger::l_log}};
void Logger::l_register(lua_State *L, Logger *logger) {
  Logger::singleton = logger;
  l_funcs.push_back({nullptr, nullptr});
  mlua_registermodule(L, "Logger", l_funcs.data());
}

} // namespace mouse