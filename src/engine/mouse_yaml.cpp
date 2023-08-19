/**
 * @file mouse_yaml.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mouse_yaml.h"

#include <filesystem>
#include <string>
#include <yaml-cpp/yaml.h>

namespace mouse {
mouse_project_data_t *myaml_load_project(std::filesystem::path path) {
  YAML::Node yaml = YAML::LoadFile(path.string());
  mouse_project_data_t *project = new mouse_project_data_t;

  if (yaml["title"]) {
    project->project_name = yaml["title"].as<std::string>();
  }
  if (yaml["scene"]) {
    project->initial_scene =
        (std::filesystem::path)yaml["scene"].as<std::string>();
  }
  return project;
}

std::string myaml_getfilename(std::filesystem::path path) {
  return path.stem().string();
}
} // namespace mouse