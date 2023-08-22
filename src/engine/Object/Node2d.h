/**
 * @file Node2d.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_NODE2D_H
#define MOUSE_NODE2D_H

#include "Node.h"
#include "Vector.h"

namespace mouse {
class Node2d : public Node {
public:
  static mouse::Type type;

  // Constructors/Destructors
  Node2d(const char *_name);
  Node2d();
  ~Node2d();

  int getX();
  void setX(int _x);
  int getY();
  void setY(int _y);
  void setPosition(int x, int y);
  void setPosition(Vector2 v);
  Vector2 getPosition();

  static void l_register(lua_State *L);
  // Lua wrappers
  static int l_new(lua_State *L);
  static int l_getX(lua_State *L);
  static int l_setX(lua_State *L);
  static int l_getY(lua_State *L);
  static int l_setY(lua_State *L);
  static int l_getPosition(lua_State *L);
  static int l_setPosition(lua_State *L);

protected:
  Vector2 position;
};
} // namespace mouse

#endif // MOUSE_NODE2D_H