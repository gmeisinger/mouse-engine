/**
 * @file types.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_TYPES_H
#define MOUSE_TYPES_H

#include <stdint.h>

namespace mouse {
class Type {
public:
  Type(const char *name, Type *parent);
  Type(const Type &) = delete;

  static Type *byName(const char *name);

  void init();
  uint32_t getId();
  const char *getName() const;

private:
  const char *name;
  Type *const parent;
  uint32_t id;
  bool inited;
};
} // namespace mouse

#endif // MOUSE_TYPES_H