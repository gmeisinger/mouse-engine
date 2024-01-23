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
  void start(Node *node);
  void stop();
  ~AsciiGraphics();

private:
  bool initialized;
  bool isVisible(Sprite *node);
  void renderTree(Node *root, Vector2 offset);
  void run();
};

} // namespace mouse

#endif // ASCII_GRAPHICS_H
