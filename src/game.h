#include <raylib.h>

#ifndef GAME_H
#define GAME_H

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define TARGET_FPS 60

#define TAM_FONT 30
#define TAM_FONT_BIG 60

typedef struct player {
	Vector2 pos;
	float size;
	Vector2 velocity;
	int can_climb;
	Rectangle hitbox;
} Player;

typedef struct map {
	char** tiles;
    Vector2 resolution;

    Vector2 spawn_pos;
	Vector2 door_pos;

	int enemies_quantity;
	Vector2* enemies_pos;

	RenderTexture2D render_texture;

	int collisions_quantity;
	Rectangle* collisions;

	int ladders_quantity;
	Rectangle* ladders;
} Map;

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

typedef struct enemy
{
	Vector2 pos;
	Vector2 spawn_pos;
	float size;
	Vector2 velocity;
	Rectangle hitbox;

} Enemy;


enum {
	TITLE = 0,
	GAMEPLAY,
	ENDING,
	RANKING,
	QUIT
};

Player* makePlayer(Vector2 pos, float tile_size, int scale);
Enemy** makeEnemies(Vector2* pos, int quantity, float tile_size, int scale);
Map* read_map(char* map_path, int tile_size, int scale);
void createMapTexture(Map* map, Vector2 margin, int tile_size, int scale);

void drawMenu();
void handleScreenSelection(int *current_screen);

void drawEntity(Rectangle entity_hitbox);
bool checkOverlap(Rectangle player, Rectangle target);
bool checkRayCollision(Vector2 origin, Vector2 ray_dir, Rectangle target, Vector2* collision_point, Vector2* collision_normal, float* t_near_hit);
bool checkPlayerCollision(Player* player, Rectangle* target, Vector2* collision_point, Vector2* collision_normal, float* contact_time, float delta_time);
bool checkEnemyCollision(Enemy* enemy, Rectangle* target, Vector2* collision_point, Vector2* collision_normal, float* contact_time, float delta_time);
void handlePlayerMovement(Player* p, Map* map, float delta_time);
void handleEnemyMovement(Enemy* enemy, Map* map, float delta_time);

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
