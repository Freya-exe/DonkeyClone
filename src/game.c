#include <raylib.h>
#include "game.h"

void drawEntity(Rectangle entity_hitbox, Texture2D tileset, int tile_x)
{

	/*
	Rectangle src = {tile_x, 0, 16, 16};
	Rectangle dest = {entity_hitbox.x, entity_hitbox.y, entity_hitbox.width, entity_hitbox.height};
	Vector2 origin = { 0, 0 };

	DrawTexturePro(tileset, src, dest, origin, 0, WHITE);
	*/

	DrawRectangleRec(entity_hitbox, GREEN);

}