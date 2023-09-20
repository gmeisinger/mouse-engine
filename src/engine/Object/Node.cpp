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

#include "mouse_yaml.h"

namespace mouse {

mouse::Type Node::type("Node", &Object::type);

Node::Node(const char *_name) {
  basetype = "Node";
  name = nullptr;
  luatype = nullptr;
  setName(_name);
  setLuaType(type.getName());
}
Node::Node() {
  basetype = "Node";
  name = nullptr;
  luatype = nullptr;
  setName(type.getName());
  setLuaType(type.getName());
}
Node::~Node() {}

char *Node::getBaseType() { return basetype; }

void Node::addChild(Node *child) {
  child->setParent(this);
  children.push_back(child);
}
std::vector<Node *> Node::getChildren() { return children; }
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
  if (name != nullptr) {
    delete[] name;
  }
  name = new char[strlen(_name) + 1];
  strcpy(name, _name);
}

const char *Node::getLuaType() { return luatype; }
void Node::setLuaType(const char *_name) {
  if (luatype != nullptr) {
    delete[] luatype;
  }
  luatype = new char[strlen(_name) + 1];
  strcpy(luatype, _name);
}

Node *Node::getParent() { return parent; }
void Node::setParent(Node *p) { parent = p; }

void Node::setScript(lua_State *L, const char *script) {
  const char *registeredScript = myaml_getfilename(script).c_str();
  // get table
  if (!mlua_istyperegistered(L, registeredScript)) {
    /* We might have a full path, try to run the script */
    mlua_loadscript(L, script);
  }
  setLuaType(registeredScript);

  mlua_gettypemetatable(L, registeredScript);
  if (!lua_istable(L, -1)) {
    // std::cout << "error" << std::endl;
  }
  // copy table
  mlua_copytable(L);
  // store in reg
  mlua_getregistry(L, REGISTRY_OBJECTS);
  lua_pushvalue(L, -2);
  setScriptRef(luaL_ref(L, -2));
}

// Lifecycle methods

void Node::run(lua_State *L, const char *method) {
  if (scriptref < 0) return;
  // get the userdata
  mlua_getobject(L, getLuaRef());
  if (!lua_isuserdata(L, -1)) {
    // std::cout << "Node lost its lua obj ref!" << std::endl;
  }

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
      // std::cout << "Could not find function " << method << std::endl;
    }
  } else {
    lua_pop(L, 2);
    // std::cout << "Could not find script table " << method << std::endl;
  }
  for (int i = 0; i < children.size(); i++) {
    if (children[i] == nullptr) {
      children.erase(children.begin() + i);
    } else {
      children[i]->run(L, method);
    }
  }
}

// Lua callbacks

#define LUA_NODE "Node"

int Node::l_addChild(lua_State *L) {
  Node *node = *(Node **)lua_touserdata(L, 1);
  Node *child = *(Node **)lua_touserdata(L, 2);
  node->addChild(child);
  return 0;
}
int Node::l_getChild(lua_State *L) {
  // get the node
  Node *nodePtr = *reinterpret_cast<Node **>(lua_touserdata(L, 1));
  Node *child;
  // check next arg
  if (lua_isinteger(L, 2)) {
    int idx = (int)lua_tointeger(L, 2);
    child = nodePtr->getChildByIndex(idx);
  } else if (lua_isstring(L, 2)) {
    const char *childname = (const char *)lua_tostring(L, 2);
    child = nodePtr->getChildByName(childname);
  } else {
    lua_pushnil(L);
    return 1;
  }
  if (child == nullptr) {
    // std::cout << "this child is nullptr!" << std::endl;
    lua_pushnil(L);
    return 1;
  }
  // We want to get the ref to this guy
  mlua_getobject(L, child->getLuaRef());
  // instead of creating a new one!
  // *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = child;
  if (!lua_isuserdata(L, -1)) {
    // std::cout << "this child is not userdata!" << std::endl;
    lua_pushnil(L);
    lua_replace(L, -2);
  }
  // Set the metatable for the userdata (shouldn't need to now)
  // mlua_setobjectmetatable(L, child->getBaseType());
  return 1;
}
int Node::l_getParent(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  Node *parent = (*nodePtr)->getParent();
  mlua_getobject(L, parent->getLuaRef());
  // *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = parent;
  // Set the metatable for the userdata
  // mlua_setobjectmetatable(L, parent->getBaseType());
  return 1;
}
int Node::l_removeChild(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // get the index
  int idx = (int)luaL_checkinteger(L, 2);
  // erase that child
  (*nodePtr)->removeChild(idx);
  return 0;
}
int Node::l_childCount(lua_State *L) {
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  lua_pushinteger(L, (*nodePtr)->childCount());
  return 1;
}
int Node::l_getName(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // return the name
  lua_pushstring(L, (*nodePtr)->getName());
  return 1;
}
int Node::l_setName(lua_State *L) {
  // get the node
  Node **nodePtr = reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // get the string
  const char *name = (const char *)luaL_checkstring(L, 2);
  // set the name
  (*nodePtr)->setName(name);
  return 0;
}
int Node::l_setScript(lua_State *L) {
  Node *nodePtr = *reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // get the string
  const char *_script = (const char *)luaL_checkstring(L, 2);
  // set the script
  nodePtr->setScript(L, _script);
  return 0;
}
int Node::l_getScript(lua_State *L) {
  Node *nodePtr = *reinterpret_cast<Node **>(lua_touserdata(L, 1));
  // use its script ref to get the instance from the registry and return it
  mlua_getobject(L, nodePtr->getScriptRef());
  return 1;
}
int Node::l_new(lua_State *L) {
  // const char *_name = luaL_checkstring(L, 1);
  Node *node = new Node();
  *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = node;
  if (lua_isstring(L, 1)) {
    node->setName((const char *)lua_tostring(L, 1));
  }
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

std::vector<luaL_Reg> Node::l_funcs = {
    {"addChild", Node::l_addChild},       {"getChild", Node::l_getChild},
    {"removeChild", Node::l_removeChild}, {"childCount", Node::l_childCount},
    {"getName", Node::l_getName},         {"setName", Node::l_setName},
    {"getParent", Node::l_getParent},     {"setScript", Node::l_setScript},
    {"getScript", Node::l_getScript}};

void Node::l_register(lua_State *L) {
  l_funcs.push_back({nullptr, nullptr});
  mlua_registertype(L, "Node", l_new, l_funcs.data());
}

}  // namespace mouse
