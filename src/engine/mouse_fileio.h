/**
 * @file mouse_fileio.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_FILEIO_H
#define MOUSE_FILEIO_H

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string>

namespace mouse {

std::string mfile_getfilename(std::filesystem::path path);
std::string myaml_getfilename(const char *path);

} // namespace mouse

#endif // MOUSE_FILEIO_H