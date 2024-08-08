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
#include "AsciiGraphics.h"
#include "EventManager.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "Sprite.h"

namespace mouse {

#define GAME_FPS 10

using StringFunctionMap =
    std::unordered_map<std::string, std::function<void *(lua_State *)>>;

static StringFunctionMap nodeGenerators;
static int typesRef;
static int objectsRef;

static void initializeTypeGenerators(lua_State *L);

static AsciiGraphics *graphics;
static EventManager *eventManager;

/**
 * @brief Initializes the Lua environment and other
 * subsystems.
 *
 * @param l The main Lua state.
 * @param project Project data.
 * @return mouse_status_t
 */
mouse_status_t Game::init(lua_State *l, mouse_project_data_t *project) {
  L = l;
  projectData = project;

  mlua_setrootpath(projectData->project_root);

  /* We want to cache refs to these repos so they don't get gc'd */
  mlua_getregistry(L, REGISTRY_TYPES);
  typesRef = luaL_ref(L, -1);
  mlua_getregistry(L, REGISTRY_OBJECTS);
  objectsRef = luaL_ref(L, -1);

  /* Register Lua game objects */
  Node::l_register(L);
  Node2d::l_register(L);
  Sprite::l_register(L);

  initializeTypeGenerators(L);

  /* Init modules */
  graphics = new AsciiGraphics();
  eventManager = new EventManager();

  /* Register Modules */
  Graphics::l_register(L, graphics);
  EventManager::l_register(L, eventManager);

  /* Load the initial scene */
  currentScene =
      loadScene(projectData->project_root / projectData->initial_scene);
  std::cout << "Init complete!" << std::endl;

  return MOUSE_STATUS_OKAY;
}
/**
 * @brief Main Game loop.
 *
 * @return mouse_status_t
 */
mouse_status_t Game::run() {
  running = true;
  int frametime = 1000 / GAME_FPS;
  // lua_gc(L, LUA_GCSTOP, 0);
  graphics->start(currentScene->getTree()); // this will start a thread
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  {
    std::lock_guard<std::mutex> lock(graphics->getMutex());
    currentScene->getTree()->run(L, "Start");
  }
  do {
    // std::cout << lua_gettop(L) << std::endl;
    auto start = std::chrono::system_clock::now();
    auto start_ms =
        std::chrono::time_point_cast<std::chrono::milliseconds>(start);
    {
      std::lock_guard<std::mutex> lock(graphics->getMutex());
      currentScene->getTree()->run(L, "Update");
    }
    // sleep
    auto end = std::chrono::system_clock::now();
    auto end_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(end);
    std::this_thread::sleep_for(std::chrono::milliseconds(frametime) -
                                std::chrono::milliseconds(end_ms - start_ms));
  } while (running);
  // lua_gc(L, LUA_GCRESTART, 0);
  graphics->stop(); // this will join the thread
  delete currentScene;
  return MOUSE_STATUS_OKAY;
}
/**
 * @brief Recursively generates a Node tree from a valid YAML node
 * from a Scene file.
 *
 * @param yaml A Parsed YAML node. (Not a Mouse engine Node x_x)
 * @return Node* The generated Node and all of its children.
 */
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
        (std::filesystem::path)yaml["script"].as<std::string>();
    std::string script = myaml_getfilename(path);
    // check if type has been registered
    if (!mlua_istyperegistered(L, script.c_str())) {
      // this will prepend the project root to the path
      mlua_loadscript(L, path.string().c_str());
    }
    node->setLuaType(script.c_str());
    // register the node
    mlua_getregistry(L, REGISTRY_OBJECTS);
    lua_pushvalue(L, -2); // get the userdata back on top
    int ref = luaL_ref(L, -2);
    node->setLuaRef(ref);
    /* copy the script table and register it */
    node->setScript(L, script.c_str());
  }
  if (yaml["name"]) {
    const char *name = yaml["name"].as<std::string>().c_str();
    node->setName(name);
  }
  if (yaml["children"] && yaml["children"].IsSequence()) {
    for (int i = 0; i < yaml["children"].size(); i++) {
      Node *child = loadTree(yaml["children"][i]["node"]); // is this ok?
      if (child != nullptr) {
        node->addChild(child);
      } else {
        // that child was nullll
      }
    }
  }
  return node;
}
/**
 * @brief Attempts to load a Scene from a YAML file at path.
 *
 * @param path Path to Scene file.
 * @return Scene* A Scene obect.
 */
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
/**
 * @brief Type Generators are used when building the tree from a
 * scene file. Depending on the type of Node, the C++ object and
 * Lua userdata will be created, and the metatable for that userdata
 * will be set.
 *
 * @param L A Lua state object.
 */
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
  nodeGenerators["Sprite"] = [](lua_State *L) -> void * {
    Sprite *node = new Sprite();
    *reinterpret_cast<Sprite **>(lua_newuserdata(L, sizeof(Sprite *))) = node;
    mlua_setobjectmetatable(L, "Sprite");
    return (void *)node;
  };
}
/**
 * @brief Executes the generator function for baseType. If
 * successful, returns a pointer to a new object of
 * type baseType.
 *
 * @param L Lua state object.
 * @param basetype Lookup string for generator. Ex "Node", "Sprite".
 * @return void* A pointer to the generated type, or nullptr
 */
void *Game::generateBaseType(lua_State *L, const char *basetype) {
  if (nodeGenerators.find(basetype) != nodeGenerators.end()) {
    return nodeGenerators[basetype](L);
  } else {
    std::cout << "No generator for " << basetype << std::endl;
    return nullptr;
  }
}

} // namespace mouse
