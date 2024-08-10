/**
 * @file EventManager.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "Node.h"
#include "mouse_lua.h"
#include <map>
#include <string>
#include <vector>

namespace mouse {

struct Subscriber {
  Node *node;
  int callback;

  Subscriber(Node *n, int cb) : node(n), callback(cb){};
};

class EventManager {
public:
  /**
   * @brief Executes all functions mapped to the event string.
   * Function arguments are in a queue somewhere TODO.
   *
   * @param event
   */
  void fire(lua_State *L, const char *event, std::vector<int> args);
  /**
   * @brief Subscribes the node to the given event string.
   *
   * @param node
   * @param event
   * @param callback_ref Reference to the cached Lua callback function.
   */
  void subscribe(Node *node, const char *event, int callback_ref);
  void unsubscribe(Node *node, const char *event);

  static void l_register(lua_State *L, EventManager *em);

protected:
  std::map<std::string, std::vector<Subscriber>> event_map;

  static EventManager *singleton;

  static int l_fire(lua_State *L);
  static int l_subscribe(lua_State *L);
  static int l_unsubscribe(lua_State *L);
  static std::vector<luaL_Reg> l_funcs;
};

} // namespace mouse

#endif