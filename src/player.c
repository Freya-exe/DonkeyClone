#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "game.h"

#define MOVE_SPEED 150
#define GRAVITY 100

Player *makePlayer(float x, float y, float tile_size) {
	Player *p = malloc(sizeof(Player));

	p->size = tile_size * 0.85;
	p->x = x;
	p->y = y;
	p->velocity = (Vector2){0, 0};
	p->can_climb = 0;
	p->hitbox = (Rectangle){x, y, p->size, p->size};
	return p;
}

bool checkOverlap(Rectangle player, Rectangle target){

	return (player.x < (target.x + target.width) && player.x > (target.x - player.width) && player.y < (target.y + target.height) && player.y > (target.y - player.height));

}

bool checkRayCollision(Vector2 origin, Vector2 ray_dir, Rectangle target, Vector2* collision_point, Vector2* collision_normal, float* t_near_hit){

	// linear equation for near and far contact times
	Vector2 t_near;
	t_near.x = (target.x - origin.x)/(ray_dir.x - origin.x);
	t_near.y = (target.y - origin.y)/(ray_dir.y - origin.y);
	Vector2 t_far;
	t_far.x = (target.x + target.width - origin.x)/(ray_dir.x - origin.x);
	t_far.y = (target.y + target.height - origin.y)/(ray_dir.y - origin.y);

	// sort closest contact time
	if (t_near.x > t_far.x){
		float aux = t_near.x;
		t_near.x = t_far.x;
		t_far.x = aux;
	}
	if (t_near.y > t_far.y){
		float aux = t_near.y;
		t_near.y = t_far.y;
		t_far.y = aux;
	}

	// no collision
	if ((t_near.x > t_far.y) || (t_near.y > t_far.x)) return false;

	*t_near_hit = fmax(t_near.x, t_near.y);
	float t_far_hit = fmin(t_far.x, t_far.y);

	// collision occurs outside the ray length
	if (t_far_hit < 0) return false;

	collision_point->x = origin.x + (*t_near_hit) * (ray_dir.x - origin.x);
	collision_point->y = origin.y + (*t_near_hit) * (ray_dir.y - origin.y);

	if (t_near.x > t_near.y)
		if (ray_dir.x - origin.x < 0) 
			*collision_normal = (Vector2){ 1, 0 };
		else
			*collision_normal = (Vector2){ -1, 0 };
	else if (t_near.x < t_near.y)
		if (ray_dir.y - origin.y < 0)
			*collision_normal = (Vector2){ 0, 1 };
		else
			*collision_normal = (Vector2){ 0, -1 };
	
	return true;

}

bool checkRecCollision(Player* player, Rectangle* target, Vector2* collision_point, Vector2* collision_normal, float* contact_time, float delta_time){


	Rectangle expanded_target;
	expanded_target.x = target->x - player->hitbox.width/2;
	expanded_target.y = target->y - player->hitbox.height/2;
	expanded_target.width = target->width + player->hitbox.width;
	expanded_target.height = target->height + player->hitbox.height;

	Vector2 origin = {player->x + player->hitbox.width/2, player->y + player->hitbox.height/2};
	Vector2 dir = {origin.x + player->velocity.x * delta_time, origin.y + player->velocity.y * delta_time};
	
	if (checkRayCollision(origin, dir, expanded_target, collision_point, collision_normal, contact_time)){
		// contact in range of collision
		if(*contact_time < 1 && *contact_time >= 0) return true;	
	}

	return false;

}

void drawPlayer(Player p)
{

	DrawRectangleRec(p.hitbox, GREEN);

}

void handlePlayerMovement(Player *player, Map *map, float delta_time)
{

	// update player rectanle position
	player->hitbox = (Rectangle){player->x, player->y, player->size, player->size};

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
	if ((player->x >= (GetScreenWidth() - player->size)) && (player->velocity.x > 0)) player->velocity.x = 0;
	if ((player->x <= 0) && (player->velocity.x < 0)) player->velocity.x = 0;

	Vector2 collision_point = { 0, 0 };
	Vector2 collision_normal = { 0, 0 };
	float contact_time = 0;

	for (int i = 0; i < map->collisions_quantity; i++){

		if (checkRecCollision(player, &map->collisions[i], &collision_point, &collision_normal, &contact_time, delta_time)){

			// counter velocity on each axis
			player->velocity.x += collision_normal.x * fabs(player->velocity.x);
			player->velocity.y += collision_normal.y * fabs(player->velocity.y);
			
		}

	}

	player->can_climb = 0;

	for (int i = 0; i < map->ladders_quantity; i++){

		if(checkOverlap(player->hitbox, map->ladders[i])){
			player->can_climb = 1;
			if(player->y < (map->ladders[i].y - player->hitbox.height)){
			player->velocity.y = fabs(player->velocity.y);
		}
		}


	}

	// apply velocity
	player->x += player->velocity.x * delta_time;
	player->y += player->velocity.y * delta_time;

}