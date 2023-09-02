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

namespace mouse {

class Node;

class Graphics {
public:
  void init() {};
  void run(Node *node) {};
  int getWidth();
  int getHeight();
  static void l_register(lua_State *L);

protected:
  
  
  int width;
  int height;

  static int l_getWidth(lua_State *L);
  static int l_getHeight(lua_State *L);
  static std::vector<luaL_Reg> l_funcs;
};

} // namespace mouse

#endif // GRAPHICS_H
