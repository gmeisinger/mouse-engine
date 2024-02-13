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

enum mouse_status_t {
  MOUSE_STATUS_OKAY,
  MOUSE_STATUS_BAD,
};

enum mouse_graphics_type_t {
  MOUSE_GRAPHICS_ASCII,
  MOUSE_GRAPHICS_SDL,
};

typedef struct {
  std::string project_name;
  std::filesystem::path initial_scene;
  std::filesystem::path project_root;
  mouse_graphics_type_t graphics_type;
} mouse_project_data_t;

} // namespace mouse

#endif // MOUSE_ENGINE_H