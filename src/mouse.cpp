/**
 * @file mouse.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Game.h"
#include "mouse_yaml.h"

#include <iostream>
#include <string>

#define TEST_SCRIPT "test.lua"

using namespace std;
using namespace mouse;

static string mouse_project_filename = "project.mouse";

static int run(int argc, const char *argv[]) {
  /* Load the project file */

  // look for path to project in args
  if (argc < 2) {
    // no args
    return 0;
  }
  filesystem::path project_path = (filesystem::path)argv[1];
  // if it doesn't end with project.mouse, append (this way the dir works too)
  if (mouse_project_filename.length() > project_path.string().length() ||
      (0 != project_path.string().rfind(mouse_project_filename,
                                        project_path.string().length() -
                                            mouse_project_filename.length()))) {
    project_path = project_path / mouse_project_filename;
  }
  cout << project_path << endl;
  mouse_project_data_t *project = myaml_load_project(project_path);
  if (project == nullptr) {
    cout << "ERROR: Failed loading project file " << project_path << endl;
    return 1;
  }
  // project is good
  project->project_root = project_path.parent_path();
  Game *game = new Game();
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  int init_status = game->init(L, project);
  int exit_status = game->run();
  cout << "Mouse Engine exiting..." << endl;
  // // set up lua
  // lua_State *L = luaL_newstate();
  // luaL_openlibs(L);
  // // Register game objects
  // Node::l_register(L);

  // create game object
  // init with L and project path
  // game will do the parsing and return something
  // all good, run game

  /*TESTS*/

  // run script
  // int error = luaL_dofile(L, TEST_SCRIPT);
  // if (error) {
  //   const char *errorMessage = lua_tostring(L, -1);
  //   // Print or handle the error message
  //   lua_pop(L, 1); // Pop the error message from the stack
  //   cout << errorMessage << endl;
  // }

  /*  We want to run the script so that our new type has been defined
      but it doesn't actually do anything. We need to instance a Node,
      create it in lua, and use the metatable we created by extending node.
      Then in our game loop we will tell each node to run it's lifecycle
      method with its corresponding userdata.
  */

  return 0;
}

int main(int argc, const char *argv[]) {
  int ret = run(argc, argv);
  return ret;
}