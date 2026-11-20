#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource_dir.h"
#include "game.h"
#include "utils.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define TAM_FONT 30
#define TAM_FONT_BIG 60

int main ()
{

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Donkey Kong");

	TraceLog(LOG_INFO, "%s", "INITIALIZED WINDOW");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	
	SetTargetFPS(120);

	int current_screen = TITLE;
	int quit = 0;
	int pause = 0;

	int scale = 2;
	int tile_size = 16;

	Map* map = read_map("map1.txt", tile_size, scale);
	Player* player = makePlayer(map->spawn_pos, tile_size, scale);
	Enemy** enemies = makeEnemies(map->enemies_pos, map->enemies_quantity, tile_size, scale);

	// game loop
	while (!quit && !WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		
		float delta_time = GetFrameTime();

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		switch (current_screen) {


			case TITLE:
				handleScreenSelection(&current_screen);
				drawMenu();
				break;

			
			case GAMEPLAY:
				if (IsKeyPressed(KEY_SPACE)) {
					pause = !pause;
				}
				if (!pause) {
					
					DrawTextureRec(map->render_texture.texture,  (Rectangle){ 0, 0, (float)map->render_texture.texture.width, -(float)map->render_texture.texture.height }, (Vector2){ 0, 0 }, WHITE);	
					
					handlePlayerMovement(player, map, delta_time);
					
					drawEntity(player->hitbox);

					for (int i = 0; i < map->enemies_quantity; i++)
					{
						handleEnemyMovement(enemies[i], delta_time);
						drawEntity(enemies[i]->hitbox);
					}
					

				} else {
					
					char pause_msg[] = "GAME PAUSED";
					double pause_msg_size = MeasureText(pause_msg, TAM_FONT_BIG);
					DrawText(pause_msg, (SCREEN_WIDTH - pause_msg_size) / 2.0, (SCREEN_HEIGHT)/2.0 - 100, TAM_FONT_BIG, MAROON);

				}
				break;


			case QUIT:
				quit = 1;
				break;
		}
	
		EndDrawing();
	}

	CloseWindow();

	free(player);
	free_map(map);
	

	return 0;
}
