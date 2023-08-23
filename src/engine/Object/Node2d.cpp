/**
 * @file Node2d.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Node2d.h"

#include <iostream>

namespace mouse {

mouse::Type Node2d::type("Node2d", &Node::type);

// Constructors/Destructors
Node2d::Node2d(const char *_name) {
  basetype = "Node2d";
  setName(_name);
  setLuaType(type.getName());
  position = Vector2(0, 0);
}
Node2d::Node2d() {
  basetype = "Node2d";
  setName(type.getName());
  setLuaType(type.getName());
  position = Vector2(0, 0);
}
Node2d::~Node2d() {}

int Node2d::getX() { return position.x; }
void Node2d::setX(int _x) { position.x = _x; }
int Node2d::getY() { return position.y; }
void Node2d::setY(int _y) { position.y = _y; }
void Node2d::setPosition(int x, int y) {
  position.x = x;
  position.y = y;
}
void Node2d::setPosition(Vector2 v) {
  position.x = v.x;
  position.y = v.y;
}
Vector2 Node2d::getPosition() { return position; }

// Lua wrappers

#define LUA_NODE2D "Node2d"

int Node2d::l_getX(lua_State *L) {
  Node2d *nodePtr = *reinterpret_cast<Node2d **>(lua_touserdata(L, 1));
  lua_pushinteger(L, nodePtr->getX());
  return 1;
}
int Node2d::l_setX(lua_State *L) {
  {
    Node2d *nodePtr = *reinterpret_cast<Node2d **>(lua_touserdata(L, 1));
    if (lua_isinteger(L, 2)) {
      nodePtr->setX(lua_tointeger(L, 2));
    }
    return 0;
  }
}
int Node2d::l_getY(lua_State *L) {
  {
    Node2d *nodePtr = *reinterpret_cast<Node2d **>(lua_touserdata(L, 1));

    lua_pushinteger(L, nodePtr->getY());
    return 1;
  }
}
int Node2d::l_setY(lua_State *L) {
  {
    Node2d *nodePtr = *reinterpret_cast<Node2d **>(lua_touserdata(L, 1));
    if (lua_isinteger(L, 2)) {
      nodePtr->setY(lua_tointeger(L, 2));
    }
    return 0;
  }
}
int Node2d::l_getPosition(lua_State *L) {
  {
    Node2d *nodePtr = *reinterpret_cast<Node2d **>(lua_touserdata(L, 1));
    Vector2 pos = nodePtr->getPosition();
    lua_pushinteger(L, pos.x);
    lua_pushinteger(L, pos.y);
    return 2;
  }
}
int Node2d::l_setPosition(lua_State *L) {
  {
    Node2d *nodePtr = *reinterpret_cast<Node2d **>(lua_touserdata(L, 1));
    if (lua_isinteger(L, 2)) {
      // this is x, y should be next
      if (lua_isinteger(L, 3)) {
        nodePtr->setPosition((int)lua_tointeger(L, 2),
                             (int)lua_tointeger(L, 3));
      } else {
        std::cout << "Bad format for l_setPosition." << std::endl;
      }
    }
    return 0;
  }
}
int Node2d::l_new(lua_State *L) {
  Node2d *node = new Node2d();
  *reinterpret_cast<Node2d **>(lua_newuserdata(L, sizeof(Node2d *))) = node;
  if (lua_isstring(L, 1)) {
    node->setName((const char *)lua_tostring(L, 1));
  }
  // Set the metatable for the userdata
  mlua_setobjectmetatable(L, LUA_NODE2D);
  // We need to register the node
  mlua_getregistry(L, REGISTRY_OBJECTS);
  lua_pushvalue(L, -2);
  node->setLuaRef(luaL_ref(L, -2));
  lua_rawgeti(L, -1, node->getLuaRef());
  return 1;
}

static const luaL_Reg node2dFuncs[] = {{"addChild", Node::l_addChild},
                                       {"getChild", Node::l_getChild},
                                       {"removeChild", Node::l_removeChild},
                                       {"childCount", Node::l_childCount},
                                       {"getName", Node::l_getName},
                                       {"setName", Node::l_setName},
                                       {"getParent", Node::l_getParent},
                                       {"setScript", Node::l_setScript},
                                       {"getPosition", Node2d::l_getPosition},
                                       {"setPosition", Node2d::l_setPosition},
                                       {"getX", Node2d::l_getX},
                                       {"setX", Node2d::l_setX},
                                       {"getY", Node2d::l_getY},
                                       {"setY", Node2d::l_setY},
                                       {nullptr, nullptr}};
void Node2d::l_register(lua_State *L) {
  mlua_registertype(L, "Node2d", Node2d::l_new, node2dFuncs);
}

} // namespace mouse