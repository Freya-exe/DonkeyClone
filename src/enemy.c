#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "game.h"

#define MOVE_SPEED 150

Enemy** makeEnemies(Vector2* positions, int quantity, float tile_size, int scale){

    Enemy** enemies = (Enemy**)malloc(quantity * sizeof(Enemy*));

    for(int i = 0; i < quantity; i++){

        enemies[i] = (Enemy*)malloc(sizeof(Enemy));

        enemies[i]->pos = positions[i];

        enemies[i]->size = tile_size * scale;

        enemies[i]->velocity = (Vector2){ 0, 0 };



        enemies[i]->hitbox = (Rectangle){
            positions[i].x,
            positions[i].y,
            tile_size*scale,
            tile_size*scale
        };

    }

    return enemies;

}

void handleEnemyMovement(Enemy* enemy, float delta_time){

    enemy->velocity.x = MOVE_SPEED;

    enemy->pos.x += enemy->velocity.x * delta_time;
	enemy->pos.y += enemy->velocity.y * delta_time;

    enemy->hitbox = (Rectangle){enemy->pos.x, enemy->pos.y, enemy->size, enemy->size};

}
