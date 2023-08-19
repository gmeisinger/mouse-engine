/**
 * @file Scene.cpp
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Scene.h"

namespace mouse {
Scene::Scene() {}
Node *Scene::getTree() { return root; }
void Scene::setTree(Node *_root) { root = _root; }
} // namespace mouse