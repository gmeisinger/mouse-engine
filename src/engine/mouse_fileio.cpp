/**
 * @file mouse_fileio.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mouse_fileio.h"

namespace mouse {

std::string mfile_getfilename(std::filesystem::path path) {
  return path.stem().string();
}
std::string mfile_getfilename(const char *path) {
  return std::filesystem::path(path).stem().string();
}

} // namespace mouse