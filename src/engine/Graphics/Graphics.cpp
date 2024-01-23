/**
 * @file Graphics.cpp
 */

#include "Graphics.h"
#include <stdexcept>

namespace mouse {

int Graphics::getWidth() { return this->width; }
int Graphics::getHeight() { return this->height; }
std::mutex &Graphics::getMutex() { return this->gfx_mutex; }

// Lua wrappers

int Graphics::l_getWidth(lua_State *L) {
  Graphics *g = *reinterpret_cast<Graphics **>(lua_touserdata(L, 1));
  lua_pushinteger(L, g->getWidth());
  return 1;
}
int Graphics::l_getHeight(lua_State *L) {
  Graphics *g = *reinterpret_cast<Graphics **>(lua_touserdata(L, 1));
  lua_pushinteger(L, g->getHeight());
  return 1;
}
std::vector<luaL_Reg> Graphics::l_funcs = {
    {"getWidth", Graphics::l_getWidth}, {"getHeight", Graphics::l_getHeight}};
void Graphics::l_register(lua_State *L) {
  l_funcs.push_back({nullptr, nullptr});
  mlua_registermodule(L, "Graphics", l_funcs.data());
}

} // namespace mouse
