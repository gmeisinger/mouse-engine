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

mouse_status_t Game::init(lua_State *l, mouse_project_data_t *project) {
  L = l;
  projectData = project;

  /* Register Lua game objects */
  Node::l_register(L);

  /* Load the initial scene */
  currentScene =
      loadScene(projectData->project_root / projectData->initial_scene);

  return MOUSE_STATUS_OKAY;
}
mouse_status_t Game::run() {
  running = true;
  int frametime = 1000 / GAME_FPS;
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
  return MOUSE_STATUS_OKAY;
}

Node *Game::loadTree(YAML::Node yaml) {
  int stack = lua_gettop(L);
  Node *node = new Node();

  if (yaml["script"]) {
    std::filesystem::path path =
        projectData->project_root /
        (std::filesystem::path)yaml["script"].as<std::string>();
    std::string type = myaml_getfilename(path);
    // check if type has been registered
    if (!mlua_istyperegistered(L, type.c_str())) {
      // run the script
      int error = luaL_dofile(L, path.string().c_str());
      if (error) {
        const char *errorMessage = lua_tostring(L, -1);
        // Print or handle the error message
        lua_pop(L, 1); // Pop the error message from the stack
        std::cout << errorMessage << std::endl;
      }
      // register the type
      mlua_registerscript(L, type.c_str());
    }
    node->setLuaType(type.c_str());
    *reinterpret_cast<Node **>(lua_newuserdata(L, sizeof(Node *))) = node;
    mlua_setobjectmetatable(L, "Node");

    // register the node
    mlua_getregistry(L, REGISTRY_OBJECTS);
    lua_pushvalue(L, -2); // get the userdata back on top
    int ref = luaL_ref(L, -2);
    node->setLuaRef(ref);
    /* copy the script table and register it */
    mlua_gettypemetatable(L, type.c_str());
    mlua_copytable(L);
    lua_pushvalue(L, -3);
    lua_pushvalue(L, -2);
    // now the stack is [registry, copy]
    int sref = luaL_ref(L, -2);
    node->setScriptRef(sref);
    // clear the stack
    lua_settop(L, 0);
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
} // namespace mouse