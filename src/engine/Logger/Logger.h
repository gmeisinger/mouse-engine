/**
 * @file Logger.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "mouse_lua.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace mouse {

class Logger {
public:
  Logger(const std::string &filename);
  ~Logger();
  void log(const std::string &msg);

  static void l_register(lua_State *L, Logger *logger);

  static Logger *singleton;

protected:
  std::ofstream logFile; // File stream for the log file

  static int l_log(lua_State *L);
  static std::vector<luaL_Reg> l_funcs;
};

} // namespace mouse

#endif // LOGGER_H