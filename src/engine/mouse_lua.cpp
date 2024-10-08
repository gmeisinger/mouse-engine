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

#include <filesystem>
#include <iostream>

namespace mouse {

static int stack_size = 0;

static std::filesystem::path root_path;

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
void mlua_setrootpath(std::filesystem::path path) { root_path = path; }
void mlua_setfuncs(lua_State *L, const luaL_Reg *l) {
  if (l == nullptr)
    return;

  for (; l->name != nullptr; l++) {
    lua_pushcfunction(L, l->func);
    lua_setfield(L, -2, l->name);
  }
}
const char *mlua_loadscript(lua_State *L, const char *path) {
  std::filesystem::path fullPath = root_path / std::filesystem::path(path);
  // run the script
  int error = luaL_dofile(L, fullPath.string().c_str());
  if (error) {
    const char *errorMessage = lua_tostring(L, -1);
    // Print or handle the error message
    lua_pop(L, 1); // Pop the error message from the stack
    std::cout << errorMessage << std::endl;
  }
  // register the type
  const char *newType = myaml_getfilename(path).c_str();
  mlua_registerscript(L, newType);
  return newType;
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
void mlua_registermodule(lua_State *L, const char *mod_name,
                         const luaL_Reg *l_funcs) {
  // lua_newtable(L);
  // lua_setglobal(L, mod_name);

  /* Modules live in the global table "Mouse"
   * They cannot be instanced, but provide static functions
   */

  luaL_newmetatable(L, mod_name);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  mlua_setfuncs(L, l_funcs);

  mlua_getregistry(L, REGISTRY_MODULES);
  lua_pushvalue(L, -2);
  lua_setfield(L, -2, mod_name);

  /* Pop the registry, leave the mt */
  lua_pop(L, 2);
}
void mlua_registertype(lua_State *L, const char *type, lua_CFunction l_new,
                       const luaL_Reg *l_funcs) {
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
  lua_pushvalue(L, -2);      // Push a copy of the metatable
  lua_setfield(L, -2, type); // _mousetypes[type] = mt

  /* pop registry and mt */
  lua_pop(L, 2);

  // now we should have a global that makes new nodes,
  // which are userdata with the metatable we registered
}
void mlua_gettypemetatable(lua_State *L, const char *type) {
  // lua_getglobal(L, "_mousetypes");

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
  if (!lua_isuserdata(L, -1)) {
    std::cout << "No userdata on stack, cannot set metatable." << std::endl;
  }
  mlua_gettypemetatable(L, type);
  if (lua_istable(L, -1)) {
    // std::cout << lua_gettop(L) << std::endl;
    // lua_pushnil(L);
    // while (lua_next(L, -2) != 0) {
    //   const char *key = (const char *)lua_tostring(L, -2); // key
    //   const char *val = (const char *)lua_tostring(L, -1); // value
    //   std::cout << key << std::endl;
    //   lua_pop(L, 1); // pop value, keep key (so it can get popped by next)
    // }
    lua_setmetatable(L, -2); // pops metatable
  } else {
    lua_pop(L, 1); // pops whatever we got from the registry (nil?)
    // type isn't registered
    std::cout << "Type name not registered, cannot set metatable." << std::endl;
  }
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

      // // metatable.__mode = "k". Weak userdata keys.
      // lua_pushliteral(L, "k");
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
    // lua_getglobal(L, "_mousetypes");
    // Create registry._mousetypes if it doesn't exist yet.
    if (!lua_istable(L, -1)) {
      lua_newtable(L);
      lua_replace(L, -2);

      // // Create a metatable.
      // lua_newtable(L);

      // // metatable.__mode = "v". Weak userdata values.
      // lua_pushliteral(L, "k");
      // lua_setfield(L, -2, "__mode");

      // // setmetatable(newtable, metatable)
      // lua_setmetatable(L, -2);

      // registry._mouseobjects = newtable
      lua_setfield(L, LUA_REGISTRYINDEX, "_mousetypes");
      lua_getfield(L, LUA_REGISTRYINDEX, "_mousetypes");
      // lua_setglobal(L, "_mousetypes");
      // lua_getglobal(L, "_mousetypes");
    }
    return 1;
  case REGISTRY_MODULES:
    lua_getglobal(L, "Mouse");
    // Create registry._mousetypes if it doesn't exist yet.
    if (!lua_istable(L, -1)) {
      lua_newtable(L);
      lua_replace(L, -2);

      // // Create a metatable.
      // lua_newtable(L);

      // // metatable.__mode = "v". Weak userdata values.
      // lua_pushliteral(L, "k");
      // lua_setfield(L, -2, "__mode");

      // // setmetatable(newtable, metatable)
      // lua_setmetatable(L, -2);

      // registry._mouseobjects = newtable
      lua_setglobal(L, "Mouse");
      lua_getglobal(L, "Mouse");
    }
    return 1;
  case REGISTRY_CALLBACKS:
    lua_getfield(L, LUA_REGISTRYINDEX, "_mousecallbacks");

    // Create registry._mousetypes if it doesn't exist yet.
    if (!lua_istable(L, -1)) {
      lua_newtable(L);
      lua_replace(L, -2);

      lua_setfield(L, LUA_REGISTRYINDEX, "_mousecallbacks");
      lua_getfield(L, LUA_REGISTRYINDEX, "_mousecallbacks");
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

} // namespace mouse
