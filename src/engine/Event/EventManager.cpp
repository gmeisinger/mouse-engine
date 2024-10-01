/**
 * @file EventManager.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "EventManager.h"
#include "Logger.h"

namespace mouse {

EventManager *EventManager::singleton = nullptr;

void EventManager::fire(lua_State *L, const char *event,
                        std::vector<int> args) {
  if (event_map.find(event) != event_map.end()) {
    for (Subscriber sub : event_map[event]) {
      // get the callback, put it on stack
      lua_rawgeti(L, LUA_REGISTRYINDEX, sub.callback);
      // put the args on the stack
      for (int arg : args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, arg);
        int type = lua_type(L, 1);
        switch (type) {
        case LUA_TNUMBER:
          break;
        case LUA_TSTRING:
          Logger::singleton->log(lua_tostring(L, 1));
          break;
        default:
          Logger::singleton->log("not a string...");
          break;
        }
      }
      // put the node ref on the stack
      mlua_getobject(L, sub.node->getLuaRef());
      // call it!
      lua_pcall(L, args.size() + 1, 0, 0);
      // release the args
      for (int arg : args) {
        luaL_unref(L, LUA_REGISTRYINDEX, arg);
      }
    }
  }
}
void EventManager::subscribe(Node *node, const char *event, int callback_ref) {
  if (event_map.find(event) == event_map.end()) {
    event_map[event] = std::vector<Subscriber>();
  }
  // TODO check that this node isn't registered already
  event_map[event].push_back(Subscriber(node, callback_ref));
}
void EventManager::unsubscribe(Node *node, const char *event) {
  if (event_map.find(event) != event_map.end()) {
    for (int i = 0; i < event_map[event].size(); i++) {
      if (event_map[event][i].node == node) {
        event_map[event].erase(event_map[event].begin() + i);
      }
    }
  }
}
/* Lua Callbacks */

int EventManager::l_fire(lua_State *L) {
  /* cache the args in a vector */
  std::vector<int> args;
  int nargs = lua_gettop(L) - 1;
  for (int i = 0; i < nargs; i++) {
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    args.push_back(ref);
  }
  // last arg is the event string
  const char *event_str = (const char *)luaL_checkstring(L, 1);
  // we need to pop this off the stack
  lua_pop(L, 1);
  /* and fire! */
  EventManager::singleton->fire(L, event_str, args);
  return 0;
}
int EventManager::l_subscribe(lua_State *L) {
  // first arg is the node
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // second arg is the event string
  const char *event_str = (const char *)luaL_checkstring(L, 2);
  // third arg is the callback, we want to cache it and use the ref
  lua_pushvalue(L, 3);
  int callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  EventManager::singleton->subscribe(*nodePtr, event_str, callback_ref);
  while (lua_gettop(L) > 0) {
    lua_pop(L, 1);
  }
  return 0;
}
int EventManager::l_unsubscribe(lua_State *L) {
  // first arg is node
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // second arg is event string
  const char *event_str = (const char *)luaL_checkstring(L, 2);

  EventManager::singleton->unsubscribe(*nodePtr, event_str);
  return 0;
}
std::vector<luaL_Reg> EventManager::l_funcs = {
    {"fire", EventManager::l_fire},
    {"subscribe", EventManager::l_subscribe},
    {"unsubscribe", EventManager::l_unsubscribe}};
void EventManager::l_register(lua_State *L, EventManager *em) {
  EventManager::singleton = em;
  l_funcs.push_back({nullptr, nullptr});
  mlua_registermodule(L, "Events", l_funcs.data());
}
} // namespace mouse