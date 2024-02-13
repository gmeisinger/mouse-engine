/**
 * @file Graphics.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "mouse_lua.h"
#include <atomic>
#include <mutex>
#include <thread>

namespace mouse {

class Node;

class Graphics {
public:
  void start(Node *node){};
  void stop(){};
  std::mutex &getMutex();
  int getWidth();
  int getHeight();

  static void l_register(lua_State *L, Graphics *g);

protected:
  static Graphics *singleton;

  void run(){};

  std::thread gfx_thread;
  std::mutex mutex;
  std::atomic<bool> running;

  int width;
  int height;
  Node *root;

  static int l_getScreenWidth(lua_State *L);
  static int l_getScreenHeight(lua_State *L);
  static std::vector<luaL_Reg> l_funcs;
};

} // namespace mouse

#endif // GRAPHICS_H
