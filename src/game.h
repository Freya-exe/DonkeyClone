#include <raylib.h>

#ifndef GAME_H
#define GAME_H

typedef struct player {
	float x;
	float y;
	float size;
	Vector2 velocity;
	int can_climb;
	Rectangle hitbox;
} Player;

typedef struct map {
	int collisions_quantity;
	Rectangle* collisions;
	int ladders_quantity;
	Rectangle* ladders;
    char** tiles;
    Vector2 resolution;
    Vector2 spawn_pos;
    Vector2 porta_pos;

} Map;

enum {
	TITLE = 0,
	GAMEPLAY,
	ENDING,
	RANKING,
	QUIT
};

Player *makePlayer(float x, float y, float size);
Map *read_map(char* map_path, int tile_size);

void drawMenu();
void handleScreenSelection(int *current_screen);

void drawPlayer(Player p);
bool checkOverlap(Rectangle player, Rectangle target);
bool checkRayCollision(Vector2 origin, Vector2 ray_dir, Rectangle target, Vector2* collision_point, Vector2* collision_normal, float* t_near_hit);
bool checkRecCollision(Player* player, Rectangle* target, Vector2* collision_point, Vector2* collision_normal, float* contact_time, float delta_time);
void handlePlayerMovement(Player *p, Map *map, float delta_time);


#endif