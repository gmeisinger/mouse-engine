/**
 * @file AsciiGraphics.cpp
 */

#include "AsciiGraphics.h"

#include <ncurses.h>

#include "mouse_objects.h"

namespace mouse {

bool AsciiGraphics::isVisible(Sprite *node) {
  Vector2 pos = node->getPosition();
  if (pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height)
    return true;
  else
    return false;
}

AsciiGraphics::~AsciiGraphics() {
  if (initialized) {
    endwin();
  }
}
void AsciiGraphics::init() {
  initialized = true;
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  nodelay(stdscr, true);
  width = COLS - 1;
  height = LINES - 1;
  refresh();
}
void AsciiGraphics::deinit() {
  running = false;
  endwin();
  initialized = false;
}
void AsciiGraphics::run(Node *root) {
  running = true;
  // while (running) {
  clear();
  renderTree(root, Vector2());
  refresh();
  //}
}
void AsciiGraphics::renderTree(Node *node, Vector2 offset) {
  Vector2 global_position(offset);
  // We only update position if we descend from Node2d
  if (Node2d *node2d = dynamic_cast<Node2d *>(node)) {
    global_position += node2d->getPosition();
    // We only draw if we descend from Sprite
    if (Sprite *sprite = dynamic_cast<Sprite *>(node2d)) {
      // We only draw within the viewport
      if (isVisible(sprite)) {
        mvaddstr(global_position.y, global_position.x,
                 sprite->getAscii().c_str());
      }
    }
  }
  for (Node *child : node->getChildren()) {
    renderTree(child, global_position);
  }
}
}  // namespace mouse
