/**
 * @file Node.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Node.h"

#include <cstring>
#include <iostream>

namespace mouse {

mouse::Type Node::type("Node", &Object::type);

Node::Node(const char *_name) {
  setName(_name);
  setLuaType(type.getName());
}
Node::Node() {
  setName(type.getName());
  setLuaType(type.getName());
}
Node::~Node() {}

void Node::addChild(Node *child) {
  child->setParent(this);
  children.push_back(child);
}

Node *Node::getChildByIndex(int idx) { return children[idx]; }
Node *Node::getChildByName(const char *childname) {
  for (int i = 0; i < children.size(); i++) {
    if (std::strcmp(children[i]->getName(), childname) == 0) {
      return children[i];
    }
  }
}

void Node::removeChild(int idx) { children.erase(children.begin() + idx); }

int Node::childCount() { return children.size(); }

const char *Node::getName() { return name; }
void Node::setName(const char *_name) {
  delete[] name;
  name = new char[strlen(_name) + 1];
  strcpy(name, _name);
}

const char *Node::getLuaType() { return luatype; }
void Node::setLuaType(const char *_name) {
  delete[] luatype;
  luatype = new char[strlen(_name) + 1];
  strcpy(luatype, _name);
}

Node *Node::getParent() { return parent; }
void Node::setParent(Node *p) { parent = p; }

void Node::setScript(lua_State *L, const char *script) {
  // get table
  if (mlua_istyperegistered(L, script)) {
    mlua_gettypemetatable(L, script);
    // copy table
    mlua_copytable(L);
    // store in reg
    mlua_getregistry(L, REGISTRY_OBJECTS);
    lua_pushvalue(L, -2);
    setScriptRef(luaL_ref(L, -2));

    // lua_settop(L, 0);
  }
}

// Lifecycle methods

void Node::run(lua_State *L, const char *method) {
  // get the userdata
  mlua_getobject(L, getLuaRef());
  // get it's script table
  mlua_getobject(L, getScriptRef());
  if (lua_istable(L, -1)) {

    // get the start function from the script
    lua_getfield(L, -1, method);
    if (lua_isfunction(L, -1)) {

      // push the table as the first argument
      lua_pushvalue(L, -2);

      // push the userdata as the second argument
      lua_pushvalue(L, -4);

      // call start
      if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        std::cout << error << std::endl;
      }
      lua_pop(L, 2);
    } else {
      lua_pop(L, 3);
      std::cout << "Could not find function " << method << std::endl;
    }
  } else {
    lua_pop(L, 2);
    std::cout << "Could not find script table " << method << std::endl;
  }
  for (int i = 0; i < children.size(); i++) {
    if (children[i] == nullptr) {
      children.erase(children.begin() + i);
    } else {

      children[i]->run(L, method);
    }
  }
}

void Node::start(lua_State *L) {
  // get the registry
  mlua_getregistry(L, REGISTRY_OBJECTS);
  if (!lua_istable(L, -1)) {
    std::cout << "Error getting registry." << std::endl;
    return;
  }
  // get the userdata
  lua_rawgeti(L, -1, getLuaRef());
  if (!lua_isuserdata(L, -1)) {
    std::cout << "Error getting userdata." << std::endl;
    return;
  }
  // get the required metatable
  // mlua_gettypemetatable(L, luatype);
  // if (!lua_istable(L, -1)) {
  //   std::cout << "Error getting metatable for " << luatype << std::endl;
  //   return;
  // }
  // get it's script table
  lua_rawgeti(L, -2, getScriptRef());
  if (!lua_istable(L, -1)) {
    std::cout << "Error getting script for " << luatype << std::endl;
    return;
  }
  // get the start function from the script
  lua_getfield(L, -1, "Start");
  if (lua_isfunction(L, -1)) {

    // push the table as the first argument
    lua_pushvalue(L, -2);

    // push the userdata as the second argument
    lua_pushvalue(L, -4);

    // call start
    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
      const char *error = lua_tostring(L, -1);
      std::cout << error << std::endl;
    }
  } else {
    std::cout << "Could not find function 'Start'." << std::endl;
  }
  for (Node *child : children) {
    child->start(L);
  }
}
void Node::update(lua_State *L) {
  // get the registry
  // mlua_getregistry(L, REGISTRY_OBJECTS);
  // if (!lua_istable(L, -1)) {
  //   std::cout << "Error getting registry." << std::endl;
  //   return;
  // }
  // // get the userdata
  mlua_getobject(L, getLuaRef());
  // lua_rawgeti(L, -1, getLuaRef());
  // if (!lua_isuserdata(L, -1)) {
  //   std::cout << "Error getting userdata." << std::endl;
  //   return;
  // }
  // get the required metatable
  // mlua_gettypemetatable(L, luatype);
  // if (!lua_istable(L, -1)) {
  //   std::cout << "Error getting metatable for " << luatype << std::endl;
  //   return;
  // }
  // get it's script table
  lua_rawgeti(L, -2, getScriptRef());
  if (!lua_istable(L, -1)) {
    std::cout << "Error getting script for " << luatype << std::endl;
    return;
  }
  // get the start function from the script
  lua_getfield(L, -1, "Update");
  if (lua_isfunction(L, -1)) {

    // push the table as the first argument
    lua_pushvalue(L, -2);
    // push the userdata as the second argument
    lua_pushvalue(L, -4);

    // call start
    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
      const char *error = lua_tostring(L, -1);
      std::cout << error << std::endl;
    }
  } else {
    std::cout << "Could not find function 'Start'." << std::endl;
  }
  for (Node *child : children) {
    child->update(L);
  }
}

// Lua callbacks

#define LUA_NODE "Node"

static int l_Node_addChild(lua_State *L) {
  Node *node = *(Node **)luaL_checkudata(L, 1, LUA_NODE);
  Node *child = *(Node **)luaL_checkudata(L, 2, LUA_NODE);
  node->addChild(child);
  return 0;
}
static int l_Node_getChild(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(luaL_checkudata(L, 1, LUA_NODE));
  Node *child;
  // check next arg
  if (lua_isstring(L, 2)) {
    const char *childname = (const char *)luaL_checkstring(L, 2);
    child = (*nodePtr)->getChildByName(childname);
  } else if (lua_isinteger(L, 2)) {
    int idx = (int)luaL_checkinteger(L, 2);
    child = (*nodePtr)->getChildByIndex(idx);
  } else {
    lua_pushnil(L);
    return 1;
  }

  // stuff him in a udata
  *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = child;
  // Set the metatable for the userdata
  luaL_setmetatable(L, LUA_NODE);
  return 1;
}
static int l_Node_getParent(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(luaL_checkudata(L, 1, LUA_NODE));
  Node *parent = (*nodePtr)->getParent();
  *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = parent;
  // Set the metatable for the userdata
  luaL_setmetatable(L, LUA_NODE);
  return 1;
}
static int l_Node_removeChild(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(luaL_checkudata(L, 1, LUA_NODE));
  // get the index
  int idx = (int)luaL_checkinteger(L, 2);
  // erase that child
  (*nodePtr)->removeChild(idx);
  return 0;
}
static int l_Node_childCount(lua_State *L) {
  Node **nodePtr = reinterpret_cast<Node **>(luaL_checkudata(L, 1, LUA_NODE));
  lua_pushinteger(L, (*nodePtr)->childCount());
  return 1;
}
static int l_Node_getName(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(luaL_checkudata(L, 1, LUA_NODE));
  // return the name
  lua_pushstring(L, (*nodePtr)->getName());
  return 1;
}
static int l_Node_setName(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(luaL_checkudata(L, 1, LUA_NODE));
  // get the string
  const char *name = (const char *)luaL_checkstring(L, 2);
  // set the name
  (*nodePtr)->setName(name);
  return 0;
}
static int l_Node_setScript(lua_State *L) {
  Node *nodePtr = *reinterpret_cast<Node **>(luaL_checkudata(L, 1, LUA_NODE));
  // get the string
  const char *_script = (const char *)luaL_checkstring(L, 2);
  // set the script
  nodePtr->setScript(L, _script);
  return 0;
}
static int l_Node_delete(lua_State *L) {
  delete *reinterpret_cast<Node **>(lua_touserdata(L, 1));
  return 0;
}
static int l_Node_new(lua_State *L) {
  const char *_name = luaL_checkstring(L, 1);
  Node *node = new Node(_name);
  *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = node;

  // Set the metatable for the userdata
  // luaL_setmetatable(L, LUA_NODE);
  mlua_setobjectmetatable(L, LUA_NODE);
  // We need to register the node
  mlua_getregistry(L, REGISTRY_OBJECTS);
  lua_pushvalue(L, -2);
  node->setLuaRef(luaL_ref(L, -2));
  lua_rawgeti(L, -1, node->getLuaRef());
  return 1;
}

// TypeData
static const luaL_Reg nodeFuncs[] = {{"addChild", l_Node_addChild},
                                     {"getChild", l_Node_getChild},
                                     {"removeChild", l_Node_removeChild},
                                     {"childCount", l_Node_childCount},
                                     {"getName", l_Node_getName},
                                     {"setName", l_Node_setName},
                                     {"getParent", l_Node_getParent},
                                     {"setScript", l_Node_setScript},
                                     {nullptr, nullptr}};
void Node::l_register(lua_State *L) {
  mlua_registertype(L, "Node", l_Node_new, l_Node_delete, nodeFuncs);
}

} // namespace mouse