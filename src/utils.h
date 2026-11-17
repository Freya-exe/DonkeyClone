#include <stdlib.h>
#include "game.h"

#ifndef UTILS_H
#define UTILS_H

void free_map(Map* map);
Vector2 tileToPosition(Vector2 coord, Vector2 margin, int tile_size);

#endif