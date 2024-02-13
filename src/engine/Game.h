/**
 * @file Game.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_GAME_H
#define MOUSE_GAME_H

#include "Scene.h"
#include "mouse_engine.h"
#include "mouse_lua.h"
#include "mouse_yaml.h"

namespace mouse {

/**
 * @brief The Game handles reading in a project, handling
 * game objects as well as the game loop.
 *
 */
class Game {
public:
  mouse_status_t init(lua_State *l, mouse_project_data_t *project);
  mouse_status_t run();
  static void *generateBaseType(lua_State *L, const char *basetype);

private:
  lua_State *L;
  bool running;
  mouse_project_data_t *projectData;
  Scene *currentScene;
  // load/save methods
  Scene *loadScene(std::filesystem::path path);
  Node *loadTree(YAML::Node yaml);
};
} // namespace mouse

#endif // MOUSE_GAME_H