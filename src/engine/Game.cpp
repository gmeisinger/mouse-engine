/**
 * @file Game.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Game.h"
#include <chrono>
#include <iostream>
#include <thread>
namespace mouse {

#define GAME_FPS 10

using StringFunctionMap =
    std::unordered_map<std::string, std::function<void *(lua_State *)>>;
static StringFunctionMap nodeGenerators;
static int typesRef;
static int objectsRef;

static void initializeTypeGenerators(lua_State *L);

mouse_status_t Game::init(lua_State *l, mouse_project_data_t *project) {
  L = l;
  projectData = project;

  /* We want to cache refs to these repos so they don'y get gc'd */
  mlua_getregistry(L, REGISTRY_TYPES);
  typesRef = luaL_ref(L, -1);
  mlua_getregistry(L, REGISTRY_OBJECTS);
  objectsRef = luaL_ref(L, -1);

  /* Register Lua game objects */
  Node::l_register(L);
  Node2d::l_register(L);

  initializeTypeGenerators(L);

  /* Load the initial scene */
  currentScene =
      loadScene(projectData->project_root / projectData->initial_scene);
  std::cout << "Init complete!" << std::endl;
  return MOUSE_STATUS_OKAY;
}
mouse_status_t Game::run() {
  running = true;
  int frametime = 1000 / GAME_FPS;
  // lua_gc(L, LUA_GCSTOP, 0);
  currentScene->getTree()->run(L, "Start");
  do {
    auto start = std::chrono::system_clock::now();
    auto start_ms =
        std::chrono::time_point_cast<std::chrono::milliseconds>(start);

    currentScene->getTree()->run(L, "Update");
    // graphics->render(currentScene->getTree());

    // sleep
    auto end = std::chrono::system_clock::now();
    auto end_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(end);
    std::this_thread::sleep_for(std::chrono::milliseconds(frametime) -
                                std::chrono::milliseconds(end_ms - start_ms));
  } while (running);
  // lua_gc(L, LUA_GCRESTART, 0);
  return MOUSE_STATUS_OKAY;
}

Node *Game::loadTree(YAML::Node yaml) {
  int stack = lua_gettop(L);
  Node *node;
  if (yaml["type"]) {
    node = (Node *)generateBaseType(L, yaml["type"].as<std::string>().c_str());

  } else {
    node = (Node *)generateBaseType(L, "Node");
  }
  if (yaml["script"]) {
    std::filesystem::path path =
        projectData->project_root /
        (std::filesystem::path)yaml["script"].as<std::string>();
    std::string script = myaml_getfilename(path);
    // check if type has been registered
    if (!mlua_istyperegistered(L, script.c_str())) {
      // run the script
      int error = luaL_dofile(L, path.string().c_str());
      if (error) {
        const char *errorMessage = lua_tostring(L, -1);
        // Print or handle the error message
        lua_pop(L, 1); // Pop the error message from the stack
        std::cout << errorMessage << std::endl;
      }
      // register the type
      mlua_registerscript(L, script.c_str());
    }
    node->setLuaType(script.c_str());
    // register the node
    mlua_getregistry(L, REGISTRY_OBJECTS);
    lua_pushvalue(L, -2); // get the userdata back on top
    int ref = luaL_ref(L, -2);
    node->setLuaRef(ref);
    /* copy the script table and register it */
    mlua_gettypemetatable(L, script.c_str());
    mlua_copytable(L);
    lua_pushvalue(L, -3);
    lua_pushvalue(L, -2);
    // now the stack is [registry, copy]
    int sref = luaL_ref(L, -2);
    node->setScriptRef(sref);
    // clear the stack
    lua_pop(L, 5);
  }
  if (yaml["name"]) {
    // std::cout << yaml["name"].as<std::string>().c_str() << std::endl;
    const char *name = yaml["name"].as<std::string>().c_str();
    node->setName(name);
  }
  if (yaml["children"] && yaml["children"].IsSequence()) {
    for (int i = 0; i < yaml["children"].size(); i++) {
      Node *child = loadTree(
          yaml["children"][i]["node"]); // this isn't gonna work for long
      if (child != nullptr) {
        node->addChild(child);
      } else {
        // that child was nullll
      }
    }
  }
  return node;
}
Scene *Game::loadScene(std::filesystem::path path) {
  std::cout << path << std::endl;
  YAML::Node yaml = YAML::LoadFile(path.string());
  if (yaml["root"]) {
    Node *root = loadTree(yaml["root"]);
    Scene *scene = new Scene();
    scene->setTree(root);
    return scene;
  } else
    return nullptr;
}
void initializeTypeGenerators(lua_State *L) {
  nodeGenerators["Node"] = [](lua_State *L) -> void * {
    Node *node = new Node();
    *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = node;
    mlua_setobjectmetatable(L, "Node");
    return (void *)node;
  };
  nodeGenerators["Node2d"] = [](lua_State *L) -> void * {
    Node2d *node = new Node2d();
    *reinterpret_cast<Node2d **>(lua_newuserdata(L, sizeof(Node2d *))) = node;
    mlua_setobjectmetatable(L, "Node2d");
    return (void *)node;
  };
}
void *Game::generateBaseType(lua_State *L, const char *basetype) {
  if (nodeGenerators.find(basetype) != nodeGenerators.end()) {
    return nodeGenerators[basetype](L);
  } else {
    std::cout << "No generator for " << basetype << std::endl;
  }
}
} // namespace mouse