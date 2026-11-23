#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource_dir.h"
#include "game.h"
#include "utils.h"
#include <string.h>
#include <time.h>

int main()
{

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	char mapa_novo[TAM_STRING_MAPA], mapa_atual[TAM_STRING_MAPA];

	strcpy(mapa_atual, "map1.txt");
	strcpy(mapa_novo, "mapX.txt");

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

	int current_screen = TITLE;
	int winner = 0;
	int quit = 0;
	int pause = 0;

	int scale = 2;
	int tile_size = 16;

	Map* map = read_map(mapa_atual, tile_size, scale);
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

	srand(time(NULL));
	
	int rand_music_num = 1 + (rand() % (N_MUSIC - 1));

	while (!quit && !WindowShouldClose()) {
		float delta_time = GetFrameTime();
		BeginDrawing();
		ClearBackground(BLACK);
		switch (current_screen) {
			case TITLE:

				//reset variables
				pause = 0;
				current_time = -1;
				winner = 0;
				updated_ranking = 0;
				inputBox->done = 0;
				strcpy(inputBox->text, "");
				inputBox->letterCount = 0;
				strcpy(mapa_atual, "map1.txt");
				strcpy(mapa_novo, "mapX.txt");
				if (IsKeyPressed(KEY_ONE)) {
					free_map(map);
					map = read_map(mapa_atual, tile_size, scale);
				}

				// reset positions 
				player->pos = map->spawn_pos;
				for (int i = 0; i < map->enemies_quantity; i++) {
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
					DrawTextureRec(map->render_texture.texture,  (Rectangle){ 0, 0, (float)map->render_texture.texture.width, -(float)map->render_texture.texture.height }, (Vector2){ 0, 0 }, WHITE);	
					
					handlePlayerMovement(player, map, delta_time);
					
					drawEntity(player->hitbox);

					current_music = music[rand_music_num];

					if (!IsMusicStreamPlaying(current_music))
						PlayMusicStream(current_music);
					UpdateMusicStream(current_music);

					for (int i = 0; i < map->enemies_quantity; i++)
					{
						handleEnemyMovement(enemies[i], map, delta_time);
						drawEntity(enemies[i]->hitbox);

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
					if (checkOverlap(player->hitbox, (Rectangle){map->door_pos.x, map->door_pos.y, tile_size*scale, tile_size*scale}))
					{
						TraceLog(LOG_INFO, "Chegou na porta! x: %.2f, y: %.2f\n", map->door_pos.x, map->door_pos.y);

						// decidir qual vai ser o proximo mapa, se tiver proximo
						if (mapa_atual[3] == '9') {
							strcpy(mapa_novo, "fim");
						} else {
							mapa_novo[3] = mapa_atual[3] + 1;
						}

						TraceLog(LOG_INFO, "mapa antigo[3]: %c\n mapa antigo[3] - '0': %d\n", mapa_atual[3], mapa_atual[3] - '0');
						TraceLog(LOG_INFO, "mapa antigo: %s\n mapa novo: %s\n", mapa_atual, mapa_novo);

						// verifica a existencia do mapa antes de ler
						int pode_ler = 1;

						char caminho[30] = "../resources/";
						strcat(caminho, mapa_novo);
						FILE *fp = fopen(caminho, "r");

						TraceLog(LOG_INFO, "caminho: %s\n", caminho);
						if (fp == NULL) {
							pode_ler = 0;
							TraceLog(LOG_INFO, "mapa nao existe. fim de jogo.\n");
						} else {
							fclose(fp);
						}

						// ler o mapa se existe
						if (pode_ler) {
							map = read_map(mapa_novo, tile_size, scale);
							strcpy(mapa_atual, mapa_novo);

							player = makePlayer(map->spawn_pos, tile_size, scale);
							enemies = makeEnemies(map->enemies_pos, map->enemies_quantity, tile_size, scale);

							PlaySound(sounds[0]);
						} else {

							winner = 1;
							current_screen = ENDING;

							strcpy(mapa_atual, "map1.txt");
							strcpy(mapa_novo, "mapX.txt");
							free_map(map);
							map = read_map(mapa_atual, tile_size, scale);

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
				player->pos = map->spawn_pos;

				for (int i = 0; i < map->enemies_quantity; i++) {
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

	free(player);

	for (int i = 0; i < map->enemies_quantity; i++) {
		free(enemies[i]);
	}

	free(enemies);

	free(inputBox->text);
	free(inputBox);
	free_map(map);

	CloseAudioDevice();
	CloseWindow();

	return 0;
}
