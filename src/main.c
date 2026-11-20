#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource_dir.h"
#include "game.h"
#include "utils.h"
#include <string.h>

#define ENEMIES 2
#define TAM_RANKING 10

int main()
{

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	double current_time = 0;	

	Vector2 playerSpawn = {SCREEN_WIDTH/2.0, SCREEN_HEIGHT/2.0};

	InputBox *inputBox = makeInputBox(150, 150, 500, 60, 20);
 
	Vector2 map_res;

	// Tell the window to use vsync and work on high DPI displays

	// Create the window and OpenGL context
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Donkey Kong");

	TraceLog(LOG_INFO, "%s", "INITIALIZED WINDOW");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	
	SetTargetFPS(TARGET_FPS);

	int current_screen = TITLE;
	int quit = 0;
	int pause = 0;

	int scale = 2;
	int tile_size = 16;

	Map* map = read_map("map1.txt", tile_size, scale);
	Player* player = makePlayer(map->spawn_pos, tile_size, scale);
	Enemy** enemies = makeEnemies(map->enemies_pos, map->enemies_quantity, tile_size, scale);

	RankingItem* ranking = makeRanking(TAM_RANKING);

	/*
	TESTES
	updateRanking(ranking, TAM_RANKING, 0, "jonatas", 2.3);
	updateRanking(ranking, TAM_RANKING, 1, "mateus", 6.1);
	updateRanking(ranking, TAM_RANKING, 2, "amanda", 6.2);
	updateRanking(ranking, TAM_RANKING, 3, "bruno", 4.3);
	updateRanking(ranking, TAM_RANKING, 4, "otavio", 12.7);
	updateRanking(ranking, TAM_RANKING, 5, "lucas", 8.7);
	updateRanking(ranking, TAM_RANKING, 6, "ronaldo", 1.7);
	updateRanking(ranking, TAM_RANKING, 7, "viviane", 0.7);
	updateRanking(ranking, TAM_RANKING, 8, "fernanda", 4.2);
	updateRanking(ranking, TAM_RANKING, 9, "joane", 1.8);
	sortRanking(ranking, TAM_RANKING);
	writeRanking(ranking, TAM_RANKING);
	*/
	
	readRanking(ranking, TAM_RANKING);
	int updated_ranking = 0;
	int empty_ranking_slots = 0;
	for (int i = 0; i < TAM_RANKING; i++) {
		if (strcmp(ranking[i].name, "[VAZIO]") == 0) {
			empty_ranking_slots++;
		}
	}

	while (!quit && !WindowShouldClose()) {
		float delta_time = GetFrameTime();
		BeginDrawing();
		ClearBackground(BLACK);
		switch (current_screen) {
			case TITLE:
				//reset variables
				pause = 0;
				updated_ranking = 0;
				inputBox->done = 0;
				strcpy(inputBox->text, "");
				inputBox->letterCount = 0;
				// reset positions 
				player->x = playerSpawn.x;
				player->y = playerSpawn.y;
				for (int i = 0; i < ENEMIES; i++) {
					enemies[i]->pos.x = enemiesSpawn[i].x;
					enemies[i]->pos.y = enemiesSpawn[i].y;
				}
				// handle menu
				handleScreenSelection(&current_screen);
				drawMenu();
				break;
			case GAMEPLAY:
				//reset variables
				updated_ranking = 0;
				inputBox->done = 0;
				strcpy(inputBox->text, "");
				inputBox->letterCount = 0;
				//pause
				if (IsKeyPressed(KEY_TAB)) {
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
					

					for (int i = 0; i < ENEMIES; i++) {
						drawEnemy(enemies[i]);
						walkEnemy(enemies[i]);
						Rectangle _playerRect = {player->x, player->y, player->hitbox.width, player->hitbox.height};
						Rectangle enemyRect = {enemies[i]->pos.x, enemies[i]->pos.y, enemies[i]->size.x, enemies[i]->size.y};
						if (CheckCollisionRecs(_playerRect, enemyRect)) {
							current_screen = ENDING;
							inputBox->done = 0;
							strcpy(inputBox->text, "");
							inputBox->letterCount = 0;
						}
					}
					
				} else {
					char pause_msg[] = "JOGO PAUSADO";
					double pause_msg_size = MeasureText(pause_msg, TAM_FONT_BIG);
					DrawText(pause_msg, (SCREEN_WIDTH - pause_msg_size) / 2.0, (SCREEN_HEIGHT)/2.0 - 100, TAM_FONT_BIG, MAROON);
					DrawText("[1] Continuar  [2] Menu  [3] Sair", (SCREEN_WIDTH - pause_msg_size) / 2.0, (SCREEN_HEIGHT)/2.0 - 20, TAM_FONT, MAROON);
					if (IsKeyPressed(KEY_ONE)) pause = 0;
					if (IsKeyPressed(KEY_TWO)) current_screen = TITLE;
					if (IsKeyPressed(KEY_THREE)) current_screen = QUIT;
				}
				break;
			case ENDING:
				// count time to finish the game
				double end_time = GetTime();
				double time_interval = end_time - current_time;
				
				// reset positions
				player->x = playerSpawn.x;
				player->y = playerSpawn.y;
				for (int i = 0; i < ENEMIES; i++) {
					enemies[i]->pos.x = enemiesSpawn[i].x;
					enemies[i]->pos.y = enemiesSpawn[i].y;
				}

				// handle input box
				if (inputBox->done == 0 && (time_interval < ranking[TAM_RANKING - 1].time || empty_ranking_slots > 0)) {
					DrawText("Novo recorde! Digite o seu nome:", 20, 20, TAM_FONT, MAROON);
					checkInput(inputBox);
					drawInputBox(inputBox);
					DrawText("Aperte [ENTER] para enviar o seu nome", 20, 400, TAM_FONT, MAROON);
				}

				if (inputBox->done == 1) {
					sortRanking(ranking, TAM_RANKING);
					drawEndingScreen();
					handleEndingSelection(&current_screen);
					int to_update = -1;
					for (int i = 0; i < TAM_RANKING; i++) {
						if (strcmp(ranking[i].name, "[VAZIO]") == 0 && updated_ranking == 0) {
							to_update = i;
							updated_ranking = 1;
							empty_ranking_slots--;
						}
					}
					if (updated_ranking == 0) {
						to_update = TAM_RANKING - 1;
						updated_ranking = 1;
					}
					if (updated_ranking == 1 && to_update >= 0) {
						updateRanking(ranking, TAM_RANKING, to_update, inputBox->text, time_interval);
					}
				}

				break;
			case RANKING:
				sortRanking(ranking, TAM_RANKING);
				for (int i = 0; i < TAM_RANKING; i++) {
					char msg[100];
					sprintf(msg, "%s:\t\t%.2fs", ranking[i].name, ranking[i].time);
					DrawText(msg, 20, i * 30, TAM_FONT, MAROON);
				}
				char *voltar_msg = "APERTE ESPACO PARA VOLTAR";
				DrawText(voltar_msg, (SCREEN_WIDTH - MeasureText(voltar_msg, TAM_FONT))/2, SCREEN_HEIGHT - 50, TAM_FONT, MAROON);
				if (IsKeyPressed(KEY_SPACE)) current_screen = TITLE;
				break;
			case QUIT:
				quit = 1;
				break;
		}
		EndDrawing();
	}
	writeRanking(ranking, TAM_RANKING);
	free(player);
	for (int i = 0; i < ENEMIES; i++) {
		free(enemies[i]);
	}
	free(inputBox->text);
	free(inputBox);
	free_map(map);

	CloseWindow();

	return 0;
}
