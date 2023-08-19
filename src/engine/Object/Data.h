/**
 * @file Data.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_DATA_H
#define MOUSE_DATA_H

#include "Object.h"

namespace mouse {
class Data : public Object {
public:
  static mouse::Type type;
  /**
   * Destructor.
   **/
  virtual ~Data() {}
};
} // namespace mouse

#endif // MOUSE_RESOURCE_H