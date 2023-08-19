/**
 * @file types.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "types.h"

#include <string>
#include <unordered_map>

namespace mouse {

static std::unordered_map<std::string, Type *> types;

Type::Type(const char *name, Type *parent)
    : name(name), parent(parent), id(0), inited(false) {}

void Type::init() {
  static uint32_t nextId = 1;

  if (inited)
    return;

  types[name] = this;
  id = nextId++;
  inited = true;

  if (!parent)
    return;
  if (!parent->inited)
    parent->init();
}

uint32_t Type::getId() {
  if (!inited)
    init();
  return id;
}

const char *Type::getName() const { return name; }

Type *Type::byName(const char *name) {
  auto pos = types.find(name);
  if (pos == types.end())
    return nullptr;
  return pos->second;
}

} // namespace mouse