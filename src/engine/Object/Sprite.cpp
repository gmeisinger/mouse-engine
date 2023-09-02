/**
 * @file Sprite.cpp
 *
 */

#include "Sprite.h"

namespace mouse {

Sprite::Sprite() {
  ascii = "?";
  rows = 1;
  cols = 1;
  width = 1;
  height = 1;
}

int Sprite::getWidth() { return width; }
void Sprite::setWidth(int w) { width = w; }
int Sprite::getHeight() { return height; }
void Sprite::setHeight(int h) { height = h; }

int Sprite::getRows() { return rows; }
void Sprite::setRows(int r) { rows = r; }
int Sprite::getCols() { return cols; }
void Sprite::setCols(int c) { cols = c; }

std::string Sprite::getAscii() { return ascii; }
void Sprite::setAscii(std::string _ascii) { ascii = _ascii; }

// Lua wrappers

#define LUA_SPRITE "Sprite"

int Sprite::l_getWidth(lua_State *L) {
  Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
  lua_pushinteger(L, spritePtr->getWidth());
  return 1;
}
int Sprite::l_setWidth(lua_State *L) {
  {
    Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
    if (lua_isinteger(L, 2)) {
      spritePtr->setWidth(lua_tointeger(L, 2));
    }
    return 0;
  }
}
int Sprite::l_getHeight(lua_State *L) {
  Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
  lua_pushinteger(L, spritePtr->getHeight());
  return 1;
}
int Sprite::l_setHeight(lua_State *L) {
  {
    Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
    if (lua_isinteger(L, 2)) {
      spritePtr->setHeight(lua_tointeger(L, 2));
    }
    return 0;
  }
}

int Sprite::l_getRows(lua_State *L) {
  Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
  lua_pushinteger(L, spritePtr->getRows());
  return 1;
}
int Sprite::l_setRows(lua_State *L) {
  {
    Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
    if (lua_isinteger(L, 2)) {
      spritePtr->setRows(lua_tointeger(L, 2));
    }
    return 0;
  }
}
int Sprite::l_getCols(lua_State *L) {
  Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
  lua_pushinteger(L, spritePtr->getCols());
  return 1;
}
int Sprite::l_setCols(lua_State *L) {
  {
    Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
    if (lua_isinteger(L, 2)) {
      spritePtr->setCols(lua_tointeger(L, 2));
    }
    return 0;
  }
}
int Sprite::l_getAscii(lua_State *L) {
  Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
  lua_pushstring(L, spritePtr->getAscii().c_str());
  return 1;
}
int Sprite::l_setAscii(lua_State *L) {
  {
    Sprite *spritePtr = *reinterpret_cast<Sprite **>(lua_touserdata(L, 1));
    if (lua_isstring(L, 2)) {
      spritePtr->setAscii(lua_tostring(L, 2));
    }
    return 0;
  }
}

int Sprite::l_new(lua_State *L) {
  Sprite *sprite = new Sprite();
  *reinterpret_cast<Sprite **>(lua_newuserdata(L, sizeof(Sprite *))) = sprite;
  if (lua_isstring(L, 1)) {
    sprite->setName((const char *)lua_tostring(L, 1));
  }
  // Set the metatable for the userdata
  mlua_setobjectmetatable(L, LUA_SPRITE);
  // We need to register the node
  mlua_getregistry(L, REGISTRY_OBJECTS);
  lua_pushvalue(L, -2);
  sprite->setLuaRef(luaL_ref(L, -2));
  lua_rawgeti(L, -1, sprite->getLuaRef());
  return 1;
}

static std::vector<luaL_Reg> spriteFuncs = {
    {"getWidth", Sprite::l_getWidth},   {"setWidth", Sprite::l_setWidth},
    {"getHeight", Sprite::l_getHeight}, {"setHeight", Sprite::l_setHeight},
    {"getRows", Sprite::l_getRows},     {"setRows", Sprite::l_setRows},
    {"getCols", Sprite::l_getCols},     {"setCols", Sprite::l_setCols},
    {"getAscii", Sprite::l_getAscii},   {"setAscii", Sprite::l_setAscii}};
void Sprite::l_register(lua_State *L) {
  // Leave the nullptr on the end
  l_funcs.insert(l_funcs.end() - 1, spriteFuncs.begin(), spriteFuncs.end());
  mlua_registertype(L, "Sprite", Sprite::l_new, l_funcs.data());
}
}  // namespace mouse
