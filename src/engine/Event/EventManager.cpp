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

namespace mouse {

EventManager *EventManager::singleton = nullptr;

void EventManager::fire(const char *event) {
  if (event_map.find(event) != event_map.end()) {
    for (auto &sub : event_map[event]) {
      // retrive function and call it
      // mlua_... gotta write it lul
      // but we can't do it here without an L ref
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
  // first arg is the event string
  const char *event_str = (const char *)luaL_checkstring(L, 1);
  // rest of args are... args!
  /* We need to:
  1. cache the args in a stack?
  2. use the string to get the callbacks
    - the string should map to an array of ints. These ints are the lua registry
  indexes for the lua functions
  3. call it with the args
    - put the function on the stack, then push all the args
    - call with lua_call or whatever, look it up
  */
  return 0;
}
int EventManager::l_subscribe(lua_State *L) {
  // first arg is the node
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // second arg is the event string
  const char *event_str = (const char *)luaL_checkstring(L, 2);
  // third arg is the callback, we want to cache it and use the ref
  int callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  EventManager::singleton->subscribe(*nodePtr, event_str, callback_ref);
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