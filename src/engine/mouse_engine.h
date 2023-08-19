/**
 * @file mouse_engine.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_ENGINE_H
#define MOUSE_ENGINE_H

#include <filesystem>
#include <string>

namespace mouse {

typedef struct {
  std::string project_name;
  std::filesystem::path initial_scene;
  std::filesystem::path project_root;
} mouse_project_data_t;

} // namespace mouse

#endif // MOUSE_ENGINE_H