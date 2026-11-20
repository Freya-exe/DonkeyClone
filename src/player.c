#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "utils.h"

#define MOVE_SPEED 150
#define GRAVITY 150

Player* makePlayer(Vector2 pos, float tile_size, int scale) {
	Player* p = malloc(sizeof(Player));

	p->size = tile_size * scale * 0.75;
	p->pos.x = pos.x;
	p->pos.y = pos.y;
	p->velocity = (Vector2){0, 0};
	p->can_climb = 0;
	p->hitbox = (Rectangle){pos.x, pos.y, p->size, p->size};
	return p;

}

void handlePlayerMovement(Player* player, Map* map, float delta_time)
{
	//reset horizontal velocity (no inertia)
	player->velocity.x = 0;
	player->velocity.y = 0;

	// movement input
	if (IsKeyDown(KEY_RIGHT)) player->velocity.x = MOVE_SPEED;
	if (IsKeyDown(KEY_DOWN) && player->can_climb) player->velocity.y = MOVE_SPEED;
	if (IsKeyDown(KEY_LEFT)) player->velocity.x = -MOVE_SPEED;
	if (IsKeyDown(KEY_UP) && player->can_climb) player->velocity.y = -MOVE_SPEED;
	if (!player->can_climb) player->velocity.y = GRAVITY;

	// screen boundaries
	if ((player->pos.x >= (GetScreenWidth() - player->size)) && (player->velocity.x > 0)) player->velocity.x = 0;
	if ((player->pos.x <= 0) && (player->velocity.x < 0)) player->velocity.x = 0;

	Vector2 collision_point = { 0, 0 };
	Vector2 collision_normal = { 0, 0 };
	float contact_time = 0;

	for (int i = 0; i < map->collisions_quantity; i++){

		if (checkRecCollision(player, &map->collisions[i], &collision_point, &collision_normal, &contact_time, delta_time)){

			// counter velocity on each axis
			player->velocity.x += collision_normal.x * fabs(player->velocity.x) * (1-contact_time);
			player->velocity.y += collision_normal.y * fabs(player->velocity.y) * (1-contact_time);
			

		}

	}

	player->can_climb = 0;

	for (int i = 0; i < map->ladders_quantity; i++){

		if(checkOverlap(player->hitbox, map->ladders[i])){
			player->can_climb = 1;
			if(player->pos.y < (map->ladders[i].y - player->hitbox.height)){
			player->velocity.y = fabs(player->velocity.y);
			}
		}
	}

	// apply velocity
	player->pos.x += player->velocity.x * delta_time;
	player->pos.y += player->velocity.y * delta_time;

	// update player rectanle position
	player->hitbox = (Rectangle){player->pos.x, player->pos.y, player->size, player->size};

}
