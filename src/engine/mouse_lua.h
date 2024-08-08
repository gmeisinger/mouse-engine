/**
 * @file mouse_lua.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_LUA_H
#define MOUSE_LUA_H

#include "mouse_yaml.h"
#include "types.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

namespace mouse {

// Forward declarations
class Object;

template <typename T> class StrongRef;

/**
 * Registries represent special tables which can be accessed with
 * mlua_getregistry.
 **/
enum Registry {
  REGISTRY_OBJECTS,   // for instanced object refs
  REGISTRY_TYPES,     // for type metatables
  REGISTRY_MODULES,   // for global module tables
  REGISTRY_CALLBACKS, // for event callbacks
};

void mlua_setrootpath(std::filesystem::path path);

/* Lua Helpers */

/**
 * Registers all functions in the array l (see luaL_Reg) into the table at the
 * top of the stack.
 **/
void mlua_setfuncs(lua_State *L, const luaL_Reg *l);
/**
 * @brief Loads and executes the lua script at path. This is only
 * intended to be used with game scripts, will throw an error
 * with others.
 *
 * @param L
 * @param path
 * @return const char*
 */
const char *mlua_loadscript(lua_State *L, const char *path);
/**
 * @brief Registers a Lua type so that it can be resused. Lua types
 * are tables as opposed to userdata.
 *
 * @param L
 * @param type
 */
void mlua_registerscript(lua_State *L, const char *type);
/**
 * @brief Registers a Lua module, which are global tables of C functions inside
 * the "Mouse" table in Lua. Ex "Mouse.Graphics".
 *
 * @param L
 * @param mod_name
 * @param l_funcs
 */
void mlua_registermodule(lua_State *L, const char *mod_name,
                         const luaL_Reg *l_funcs);
// void mlua_registercallback(lua_State *L);
/**
 * Registers a base type for use in Lua. Base types represent C++ classes.
 * They are registered at boot and are available in all scripts.
 *
 * @param name The lua table will be referenced by this
 *  name in the global scope.
 * @param l_new A lua wrapped constructor function.
 * @param l_delete A lua wrapped destructor function.
 * @param l_funcs An array of luaL_Reg structs which contain
 *  a function name paired with a lua wrapped function pointer.
 */
void mlua_registertype(lua_State *L, const char *name, lua_CFunction l_new,
                       const luaL_Reg *l_funcs);
/**
 * Returns true if type has already been registered in lua.
 */
bool mlua_istyperegistered(lua_State *L, const char *type);
/**
 * @brief Gets the metatable associated with type. This can be a base type
 * like Node or a Lua type.
 *
 * @param L
 * @param type
 */
void mlua_gettypemetatable(lua_State *L, const char *type);
/**
 * @brief Sets the metatable for the userdata at the top of the Lua stack.
 * The metatable must first be registered with mlua_registertype.
 *
 * @param L
 * @param type
 */
void mlua_setobjectmetatable(lua_State *L, const char *type);
/**
 * Retrieves an object from the object registry using an integer
 * ref provided by luaL_ref.
 */
void mlua_getobject(lua_State *L, int ref);
/**
 * @brief Copies the table on top of the stack and pushes the result.
 *
 * @param L
 * @param type
 */
void mlua_copytable(lua_State *L);
/**
 * @brief Returns the registry r. If the registry has not yet been created
 * in Lua, it will be created first.
 *
 * @param L
 * @param r
 * @return int
 */
int mlua_getregistry(lua_State *L, Registry r);
} // namespace mouse

#endif // MOUSE_LUA_H
