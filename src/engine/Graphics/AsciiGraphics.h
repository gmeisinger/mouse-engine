/**
 * @file AsciiGraphics.h
 */

#ifndef ASCII_GRAPHICS_H
#define ASCII_GRAPHICS_H

#include "Graphics.h"

namespace mouse {

class Node;
class Sprite;
class Vector2;

class AsciiGraphics : public Graphics {
 public:
  void init();
  void deinit();
  void run(Node *root);
  void renderTree(Node *root, Vector2 offset);

  ~AsciiGraphics();
 private:
  bool initialized;
  bool running;
  bool isVisible(Sprite *node);
};

}  // namespace mouse

#endif  // ASCII_GRAPHICS_H
