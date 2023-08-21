/**
 * @file mouse_lua.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mouse_lua.h"
#include "Object.h"

#include <iostream>

namespace mouse {

static std::vector<std::string> registered_types = std::vector<std::string>();
static int stack_size = 0;

/**
 * Called when an object is collected. The object is released
 * once in this function, possibly deleting it.
 **/
static int m__gc(lua_State *L) {
  Object *object = (Object *)lua_touserdata(L, 1);
  if (object != nullptr) {
    object->release();
  }
  return 0;
}
static int m__release(lua_State *L) {
  Object *object = (Object *)lua_touserdata(L, 1);

  if (object != nullptr) {
    int lref = object->getLuaRef();
    int sref = object->getScriptRef();
    object->release();

    // Fetch the registry table of instantiated objects.
    mlua_getregistry(L, REGISTRY_OBJECTS);

    if (lua_istable(L, -1)) {
      luaL_unref(L, -1, lref);
      luaL_unref(L, -1, sref);
    }

    lua_pop(L, 1);
  }

  lua_pushboolean(L, object != nullptr);
  return 1;
}

void mlua_setfuncs(lua_State *L, const luaL_Reg *l) {
  if (l == nullptr)
    return;

  for (; l->name != nullptr; l++) {
    lua_pushcfunction(L, l->func);
    lua_setfield(L, -2, l->name);
  }
}

void mlua_registerscript(lua_State *L, const char *type) {
  // since we just ran the script, the derived type should be in global scope
  lua_getglobal(L, type);

  /* Register the metatable in the registry for reuse */
  mlua_getregistry(L, REGISTRY_TYPES);
  lua_pushvalue(L, -2);      // Push a copy of the table
  lua_setfield(L, -2, type); // _mousetypes[type] = mt
  lua_pop(L, 2);
}
void mlua_registertype(lua_State *L, const char *type, lua_CFunction l_new,
                       lua_CFunction l_delete, const luaL_Reg *l_funcs) {
  lua_settop(L, 0);
  /* Check if a type is registered under this name */
  if (mlua_istyperegistered(L, type)) {
    // type exists
    std::cout << "Type name already registered, cannot register type."
              << std::endl;
    return;
  }
  /* Create the global table and constructor */
  lua_newtable(L);
  lua_pushcfunction(L, l_new);
  lua_setfield(L, -2, "new");
  lua_setglobal(L, type);

  /* Create the metatable */
  luaL_newmetatable(L, type);
  // lua_newtable(L);
  // lua_pushstring(L, type);
  // lua_setfield(L, -2, "__name");

  /* Garbage collection */
  lua_pushcfunction(L, m__release);
  lua_setfield(L, -2, "__gc");

  /* Copies the mt and sets it as prototype */
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  /* Register exposed funcs */
  mlua_setfuncs(L, l_funcs);

  stack_size = lua_gettop(L);

  /* Register the metatable in the registry for reuse */
  mlua_getregistry(L, REGISTRY_TYPES);
  lua_pushvalue(L, -2); // Push a copy of the metatable
  // mlua_gettypemetatable(L, type);
  lua_setfield(L, -2, type); // _mousetypes[type] = mt

  /* pop registry and mt */
  lua_pop(L, 2);

  // now we should have a global that makes new nodes,
  // which are userdata with the metatable we registered
}
void mlua_gettypemetatable(lua_State *L, const char *type) {
  mlua_getregistry(L, REGISTRY_TYPES);
  lua_getfield(L, -1, type);
  lua_replace(L, -2);
  // this pops the registry but leaves the metatable on the stack
}
bool mlua_istyperegistered(lua_State *L, const char *type) {
  mlua_gettypemetatable(L, type);
  if (lua_istable(L, -1)) {
    lua_pop(L, 1);
    return true;
  } else {
    lua_pop(L, 1);
    return false;
  }
}
void mlua_setobjectmetatable(lua_State *L, const char *type) {
  // the userdata should be on top of the stack
  mlua_getregistry(L, REGISTRY_TYPES);
  lua_getfield(L, -1, type);
  if (lua_istable(L, -1)) {
    lua_setmetatable(L, -3); // pops metatable
  } else {
    lua_pop(L, 1); // pops whatever we got from the registry (nil?)
    // type isn't registered
    std::cout << "Type name not registered, cannot set metatable." << std::endl;
  }
  lua_pop(L, 1); // pop the registry
}
void mlua_getobject(lua_State *L, int ref) {
  mlua_getregistry(L, REGISTRY_OBJECTS);
  lua_rawgeti(L, -1, ref);
  // check if its userdata or something
  // pop the registry, leave the userdata
  lua_replace(L, -2);
}
int mlua_getregistry(lua_State *L, Registry r) {
  switch (r) {
  case REGISTRY_OBJECTS:
    lua_getfield(L, LUA_REGISTRYINDEX, "_mouseobjects");
    // Create registry._mouseobjects if it doesn't exist yet.
    if (!lua_istable(L, -1)) {
      lua_newtable(L);
      lua_replace(L, -2);

      // // Create a metatable.
      // lua_newtable(L);

      // // metatable.__mode = "v". Weak userdata values.
      // lua_pushliteral(L, "v");
      // lua_setfield(L, -2, "__mode");

      // // setmetatable(newtable, metatable)
      // lua_setmetatable(L, -2);

      // registry._mouseobjects = newtable
      lua_setfield(L, LUA_REGISTRYINDEX, "_mouseobjects");
      lua_getfield(L, LUA_REGISTRYINDEX, "_mouseobjects");
    }
    return 1;
  case REGISTRY_TYPES:
    lua_getfield(L, LUA_REGISTRYINDEX, "_mousetypes");
    // Create registry._mousetypes if it doesn't exist yet.
    if (!lua_istable(L, -1)) {
      lua_newtable(L);
      lua_replace(L, -2);

      // // Create a metatable.
      // lua_newtable(L);

      // // metatable.__mode = "v". Weak userdata values.
      // lua_pushliteral(L, "v");
      // lua_setfield(L, -2, "__mode");

      // // setmetatable(newtable, metatable)
      // lua_setmetatable(L, -2);

      // registry._mouseobjects = newtable
      lua_setfield(L, LUA_REGISTRYINDEX, "_mousetypes");
      lua_getfield(L, LUA_REGISTRYINDEX, "_mousetypes");
    }
    return 1;
  default:
    return luaL_error(L, "Attempted to use invalid registry.");
  }
}
void mlua_copytable(lua_State *L) {
  if (lua_istable(L, -1)) {
    lua_newtable(L);
    lua_pushnil(L);
    while (lua_next(L, -3) != 0) {
      lua_pushvalue(L, -2); // key
      lua_pushvalue(L, -2); // value
      lua_settable(L, -5);  // new table
      lua_pop(L, 1); // pop value, keep key (so it can get popped by next)
    }
  }
}

int mlua_insist(lua_State *L, int idx, const char *k) {
  // Convert to absolute index if necessary.
  if (idx < 0 && idx > LUA_REGISTRYINDEX)
    idx += lua_gettop(L) + 1;

  lua_getfield(L, idx, k);

  // Create if necessary.
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1); // Pop the non-table.
    lua_newtable(L);
    lua_pushvalue(L, -1);    // Duplicate the table to leave on top.
    lua_setfield(L, idx, k); // lua_stack[idx][k] = lua_stack[-1] (table)
  }

  return 1;
}

int mlua_insistglobal(lua_State *L, const char *k) {
  lua_getglobal(L, k);

  if (!lua_istable(L, -1)) {
    lua_pop(L, 1); // Pop the non-table.
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, k);
  }

  return 1;
}

} // namespace mouse