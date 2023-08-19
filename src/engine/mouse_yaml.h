/**
 * @file mouse_yaml.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_YAML_H
#define MOUSE_YAML_H

#include "mouse_engine.h"
#include <yaml-cpp/yaml.h>

namespace mouse {
mouse_project_data_t *myaml_load_project(std::filesystem::path path);
std::string myaml_getfilename(std::filesystem::path path);
} // namespace mouse

#endif // MOUSE_YAML_H