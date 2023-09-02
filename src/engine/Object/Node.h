/**
 * @file Node.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NODE_H
#define NODE_H

#include "Object.h"

#include <vector>

namespace mouse {
class Node : public Object {
public:
  static mouse::Type type;

  // Constructors/Destructors
  Node(const char *_name);
  Node();
  ~Node();

  // Node/Tree methods
  void addChild(Node *child);
  std::vector<Node *> getChildren();
  Node *getChildByIndex(int idx);
  Node *getChildByName(const char *childname);
  Node *getParent();
  void setParent(Node *p);
  void removeChild(int idx);
  int childCount();
  const char *getName();
  void setName(const char *_name);
  const char *getLuaType();
  void setLuaType(const char *_name);
  void setScript(lua_State *L, const char *script);
  char *getBaseType();

  // Lifecycle methods
  void run(lua_State *L, const char *method);
  // void start(lua_State *L);
  // void update(lua_State *L);

  // Registers the Node class in Lua
  static void l_register(lua_State *L);

  // Lua wrappers
  static int l_new(lua_State *L);
  static int l_setScript(lua_State *L);
  static int l_setName(lua_State *L);
  static int l_getName(lua_State *L);
  static int l_childCount(lua_State *L);
  static int l_removeChild(lua_State *L);
  static int l_getParent(lua_State *L);
  static int l_getChild(lua_State *L);
  static int l_addChild(lua_State *L);

protected:
  std::vector<Node *> children;
  Node *parent;
  char *name;
  char *luatype;
  char *basetype;
  static std::vector<luaL_Reg> l_funcs;
};

} // namespace mouse

#endif // NODE_H
