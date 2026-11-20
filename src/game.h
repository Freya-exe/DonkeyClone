#include <raylib.h>

#ifndef GAME_H
#define GAME_H

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define TARGET_FPS 60

#define TAM_FONT 30
#define TAM_FONT_BIG 60

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

typedef struct enemy {
	Vector2 pos;
	Vector2 size;
	double velocity;
} Enemy;

typedef struct ranking_item {
	char name[20];
	double time;
} RankingItem;

typedef struct input_box {
	Rectangle box;
	int mouseOnText;
	char *text;
	int maxText;
	int letterCount;
	int done;
} InputBox;

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
void handlePlayerMovement(Player *p, Map *map, float delta_time);

bool checkOverlap(Rectangle player, Rectangle target);
bool checkRayCollision(Vector2 origin, Vector2 ray_dir, Rectangle target, Vector2* collision_point, Vector2* collision_normal, float* t_near_hit);
bool checkRecCollision(Player* player, Rectangle* target, Vector2* collision_point, Vector2* collision_normal, float* contact_time, float delta_time);

Enemy *makeEnemy(Vector2 pos, Vector2 size, double velocity);
void walkEnemy(Enemy *e);
void drawEnemy(Enemy *e);

void drawEndingScreen();
void handleEndingSelection(int *current_screen);

RankingItem *makeRanking(int n);
void updateRanking(RankingItem *r, int n, int index, char *name, double time);
void sortRanking(RankingItem *r, int n);
void writeRanking(RankingItem *r, int n);
void readRanking(RankingItem *r, int n);

InputBox *makeInputBox(double x, double y, double width, double height, int maxText);
void checkInput(InputBox *inputBox);
void drawInputBox(InputBox *inputBox);

#endif
