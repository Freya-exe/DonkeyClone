#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource_dir.h"
#include "game.h"
#include "utils.h"
#include <string.h>
#include <time.h>

#define MAX_MAPS 10

int main()
{

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	double current_time = -1;	

	InputBox *inputBox = makeInputBox(150, 150, 500, 60, 20);

	// Create the window and OpenGL context
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Donkey Kong");

	TraceLog(LOG_INFO, "%s", "INITIALIZED WINDOW");

	InitAudioDevice();

	Sound sounds[MAX_SOUNDS] = {0};
	loadSounds(sounds);

	Music music[MAX_MUSIC] = {0};
	loadMusic(music);

	Music current_music;

	SetMasterVolume(AUDIO_VOLUME);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	
	SetTargetFPS(TARGET_FPS);

	Texture2D tileset = LoadTexture("tileset.png");

	int current_screen = TITLE;
	int winner = 0;
	int quit = 0;
	int pause = 0;

	int scale = 2;
	int tile_size = 16;

	int map_num = 0;
	int map_exists = 1;
	int cur_map = 0;

	Map** maps = (Map**)malloc(MAX_MAPS*sizeof(maps));

	for (int i = 0; i < MAX_MAPS; i++){

		maps[i] = (Map*)malloc(sizeof(Map));

	}

	do{

		char map_name[10];

		snprintf(map_name, sizeof(map_name), "%s%d", "mapa", map_num+1);
		strcat(map_name, ".txt");
		
		FILE* map_file = fopen(map_name, "r");

		if (map_file == NULL){

			map_exists = 0;

		} else{

			maps[map_num] = read_map(map_file, tile_size, scale);

			map_num++;

		}

	} while(map_exists);

	printf("map qty: %d\n", map_num);

	Player* player = makePlayer(maps[cur_map]->spawn_pos, tile_size, scale);
	Enemy** enemies = makeEnemies(maps[cur_map]->enemies_pos, maps[cur_map]->enemies_quantity, tile_size, scale);

	RankingItem* ranking = makeRanking(TAM_RANKING);
	
	readRanking(ranking, TAM_RANKING);
	int updated_ranking = 0;
	int empty_ranking_slots = 0;
	for (int i = 0; i < TAM_RANKING; i++) {
		if (strcmp(ranking[i].name, "[VAZIO]") == 0) {
			empty_ranking_slots++;
		}
	}

	srand(time(NULL));
	
	int rand_music_num = 1 + (rand() % (N_MUSIC - 1));

	while (!quit && !WindowShouldClose()) {
		float delta_time = GetFrameTime();
		BeginDrawing();
		ClearBackground(BLACK);



		switch (current_screen) {
			case TITLE:
				
				if (IsKeyPressed(KEY_ONE)) {

					free_enemies(enemies, maps[cur_map]->enemies_quantity);

					//reset variables
					pause = 0;
					current_time = -1;
					winner = 0;
					updated_ranking = 0;
					inputBox->done = 0;
					strcpy(inputBox->text, "");
					inputBox->letterCount = 0;
					cur_map = 0;
				}

				enemies = makeEnemies(maps[cur_map]->enemies_pos, maps[cur_map]->enemies_quantity, tile_size, scale);

				// reset positions 
				player->pos = maps[cur_map]->spawn_pos;
				for (int i = 0; i < maps[cur_map]->enemies_quantity; i++) {
					enemies[i]->pos = enemies[i]->spawn_pos;
				}

				// music
				handleTitleMusic(music[0]);
				current_music = music[0];

				// handle menu
				handleScreenSelection(&current_screen);
				handleTitleSounds(sounds);
				drawMenu();

				break;

			case GAMEPLAY:
			
				//reset variables
				if(current_time == -1) {
					current_time = GetTime();
				}
				updated_ranking = 0;
				winner = 0;
				inputBox->done = 0;
				strcpy(inputBox->text, "");
				inputBox->letterCount = 0;
				//pause
				if (IsKeyPressed(KEY_TAB)) {
					pause = !pause;
				}
				if (!pause) {

					DrawTextureRec(maps[cur_map]->render_texture.texture,  (Rectangle){ 0, 0, (float)maps[cur_map]->render_texture.texture.width, -(float)maps[cur_map]->render_texture.texture.height }, (Vector2){ 0, 0 }, WHITE);	
					
					handlePlayerMovement(player, maps[cur_map], delta_time);
					
					drawEntity(player->hitbox, tileset, tile_size * 3);

					current_music = music[rand_music_num];

					if (!IsMusicStreamPlaying(current_music))
						PlayMusicStream(current_music);
					UpdateMusicStream(current_music);

					for (int i = 0; i < maps[cur_map]->enemies_quantity; i++)
					{
						handleEnemyMovement(enemies[i], maps[cur_map], delta_time);
						drawEntity(enemies[i]->hitbox, tileset, tile_size * 4);

						if (checkOverlap(player->hitbox, enemies[i]->hitbox)) {
							current_screen = ENDING;
							winner = 0;
							inputBox->done = 0;
							strcpy(inputBox->text, "");
							inputBox->letterCount = 0;
							PlaySound(sounds[4]);
							PlaySound(sounds[3]);
						}
					}

					// mudar mapa, map->porta deve ter uma nova localizacao baseada no proximo mapa
					// player e inimigos devem ir para suas posicoes iniciais do novo mapa...
					if (checkOverlap(player->hitbox, (Rectangle){maps[cur_map]->door_pos.x, maps[cur_map]->door_pos.y, tile_size*scale, tile_size*scale}))
					{
						TraceLog(LOG_INFO, "Chegou na porta! x: %.2f, y: %.2f\n", maps[cur_map]->door_pos.x, maps[cur_map]->door_pos.y);

						// ler o mapa se existe
						if (cur_map < map_num) {

							free_enemies(enemies, maps[cur_map]->enemies_quantity);

							cur_map++;

							player = makePlayer(maps[cur_map]->spawn_pos, tile_size, scale);
							
							enemies = makeEnemies(maps[cur_map]->enemies_pos, maps[cur_map]->enemies_quantity, tile_size, scale);

							PlaySound(sounds[0]);

						} else {

							winner = 1;
							current_screen = ENDING;

							PlaySound(sounds[2]);
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

					// music
					if (IsKeyPressed(KEY_ONE)) ResumeMusicStream(current_music);
					if (IsKeyPressed(KEY_TWO)) StopMusicStream(current_music);
				}

				break;

			case ENDING:
				if (IsMusicStreamPlaying(current_music))
					StopMusicStream(current_music);

				rand_music_num = 1 + (rand() % (N_MUSIC - 1));
				
				// count time to finish the game
				double end_time, time_interval;
				if (current_time != -1) {
					end_time = GetTime();
					time_interval = end_time - current_time;
					current_time = -1;
				}
				printf("Time interval: %.2fs\n", time_interval);
				
				
				// reset positions
				player->pos = maps[cur_map]->spawn_pos;

				for (int i = 0; i < maps[cur_map]->enemies_quantity; i++) {
					enemies[i]->pos = enemies[i]->spawn_pos;
				}

				if (winner) {
					// handle input box
					if (inputBox->done == 0 && (time_interval < ranking[TAM_RANKING - 1].time || empty_ranking_slots > 0)) {
						DrawText("Novo recorde! Digite o seu nome:", 20, 20, TAM_FONT, MAROON);
						checkInput(inputBox);
						drawInputBox(inputBox);
						DrawText("Aperte [ENTER] para enviar o seu nome", 20, 400, TAM_FONT, MAROON);
					} else {
						current_screen = TITLE;
					}

					if (inputBox->done == 1) {
						sortRanking(ranking, TAM_RANKING);

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
							current_screen = TITLE;
						}

					}
				} else {
					drawEndingScreen();
					handleEndingSelection(&current_screen);
					handleEndingSounds(sounds);
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
				if (IsKeyPressed(KEY_SPACE)) PlaySound(sounds[1]);
				break;

			case QUIT:
				quit = 1;
				break;
		}
		EndDrawing();
	}
	
	writeRanking(ranking, TAM_RANKING);

	free(inputBox->text);
	free(inputBox);

	free(player);

	for(int i = 0; i < map_num; i++){

		for (int i = 0; i < maps[i]->enemies_quantity; i++) {
			free(enemies[i]);
		}

		free_map(maps[i]);

	}

	free(enemies);
	free(maps);

	CloseAudioDevice();
	CloseWindow();

	return 0;
}
