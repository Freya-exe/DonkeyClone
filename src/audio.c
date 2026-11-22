#include "game.h"
#include <raylib.h>
#include <string.h>

void loadSounds(Sound *sounds)
{
	char string[MAX_SOUNDS][100] = {
		"floraphonic-arcade-ui-14-229514.mp3",
		"freesound_community-menu-selection-102220.mp3",
		"krnbeatz-8bit_achievement_unlock-586749.mp3",
		"lesiakower-8-bit-game-over-sound-effect-331435.mp3",
		"soundreality-explosion-8-bit-8-314694.mp3"
	};
	
	char path[100];
	for (int i = 0; i < MAX_SOUNDS; i++) {
		strcpy(path, AUDIO_PATH);
		strcat(path, string[i]);
		sounds[i] = LoadSound(path);
	}
}

void loadMusic(Music *music)
{
	char string[MAX_MUSIC][100] = {
		"lesiakower-waiting-time-175800.mp3",
		"inono777-chiptune-but-461573.mp3",
		"reganati-chillcalming-chiptune-loop-527182.mp3",
		"suitedfrogds-8-bit-chiptune-2-400593.mp3"
	};
	
	char path[100];
	for (int i = 0; i < MAX_SOUNDS; i++) {
		strcpy(path, MUSIC_PATH);
		strcat(path, string[i]);
		music[i] = LoadMusicStream(path);
	}

}
