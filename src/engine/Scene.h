/**
 * @file Scene.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_SCENE_H
#define MOUSE_SCENE_H

#include "mouse_objects.h"

namespace mouse {
class Scene {
public:
  Scene();
  Node *getTree();
  void setTree(Node *_root);

private:
  Node *root;
};
} // namespace mouse

#endif // MOUSE_SCENE_H