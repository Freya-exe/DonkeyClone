#include <stdio.h>
#include <raylib.h>
#include "game.h"

#define TAM_FONT 30
#define MENU_X 20
#define MENU_Y 20

#define MUSIC_VOLUME 0.5

void drawMenu()
{
	DrawText("DONKEY KONG MENU", MENU_X, MENU_Y, TAM_FONT, MAROON);
	DrawText("1. NOVO JOGO", MENU_X, MENU_Y + 100, TAM_FONT, MAROON);
	DrawText("2. RANKING",MENU_X, MENU_Y + 200, TAM_FONT, MAROON);
	DrawText("3. SAIR", MENU_X, MENU_Y + 300, TAM_FONT, MAROON);
}

void handleScreenSelection(int *current_screen)
{
	if (IsKeyPressed(KEY_ONE)) *current_screen = GAMEPLAY;
	if (IsKeyPressed(KEY_TWO)) *current_screen = RANKING;
	if (IsKeyPressed(KEY_THREE)) *current_screen = QUIT;
}

void handleTitleSounds(Sound *sounds)
{
	if (IsKeyPressed(KEY_ONE)) PlaySound(sounds[1]);
	if (IsKeyPressed(KEY_TWO)) PlaySound(sounds[1]);
	if (IsKeyPressed(KEY_THREE)) PlaySound(sounds[1]);
}

void handleTitleMusic(Music music)
{
	if (!IsMusicStreamPlaying(music)) {
		PlayMusicStream(music);
		SetMusicVolume(music, MUSIC_VOLUME);
	}
	UpdateMusicStream(music);

	if (IsKeyPressed(KEY_ONE)) StopMusicStream(music);
	if (IsKeyPressed(KEY_TWO)) StopMusicStream(music);
	if (IsKeyPressed(KEY_THREE)) StopMusicStream(music);
	
}
