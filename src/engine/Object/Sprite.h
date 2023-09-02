/**
 * @file Sprite.h
 *
 */

#ifndef SPRITE_H
#define SPRITE_H

#include "Node2d.h"

namespace mouse {

class Sprite : public Node2d {
 public:
  static mouse::Type type;

  Sprite();
  // Sprite(const char *_name);
  //~Sprite();

  int getWidth();
  void setWidth(int w);
  int getHeight();
  void setHeight(int h);

  int getRows();
  void setRows(int r);
  int getCols();
  void setCols(int c);

  std::string getAscii();
  void setAscii(std::string _ascii);

  static void l_register(lua_State *L);
  // Lua wrappers
  static int l_new(lua_State *L);

  static int l_getWidth(lua_State *L);
  static int l_setWidth(lua_State *L);
  static int l_getHeight(lua_State *L);
  static int l_setHeight(lua_State *L);
  static int l_getRows(lua_State *L);
  static int l_setRows(lua_State *L);
  static int l_getCols(lua_State *L);
  static int l_setCols(lua_State *L);
  static int l_getAscii(lua_State *L);
  static int l_setAscii(lua_State *L);

 protected:
  std::string ascii;
  int width;
  int height;
  int rows;
  int cols;
};

}  // namespace mouse

#endif  // SPRITE_H
