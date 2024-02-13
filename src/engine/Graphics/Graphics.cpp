/**
 * @file Graphics.cpp
 */

#include "Graphics.h"
#include <stdexcept>

namespace mouse {

Graphics *Graphics::singleton = nullptr;

int Graphics::getWidth() { return this->width; }
int Graphics::getHeight() { return this->height; }
std::mutex &Graphics::getMutex() { return this->mutex; }

// Lua wrappers

int Graphics::l_getScreenWidth(lua_State *L) {
  // Graphics *g = *reinterpret_cast<Graphics **>(lua_touserdata(L, 1));
  lua_pushinteger(L, singleton->getWidth());
  return 1;
}
int Graphics::l_getScreenHeight(lua_State *L) {
  // Graphics *g = *reinterpret_cast<Graphics **>(lua_touserdata(L, 1));
  lua_pushinteger(L, singleton->getHeight());
  return 1;
}
std::vector<luaL_Reg> Graphics::l_funcs = {
    {"getScreenWidth", Graphics::l_getScreenWidth},
    {"getScreenHeight", Graphics::l_getScreenHeight}};
void Graphics::l_register(lua_State *L, Graphics *g) {
  Graphics::singleton = g;
  l_funcs.push_back({nullptr, nullptr});
  mlua_registermodule(L, "Graphics", l_funcs.data());
}

} // namespace mouse
