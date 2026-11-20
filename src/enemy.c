#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "game.h"

#define MOVE_SPEED 150
#define GRAVITY 150

Enemy** makeEnemies(Vector2* positions, int quantity, float tile_size, int scale){

    Enemy** enemies = (Enemy**)malloc(quantity * sizeof(Enemy*));

    for(int i = 0; i < quantity; i++){

        enemies[i] = (Enemy*)malloc(sizeof(Enemy));

        enemies[i]->pos = positions[i];

        enemies[i]->spawn_pos = positions[i];

        enemies[i]->size = tile_size * scale;

        enemies[i]->velocity = (Vector2){ MOVE_SPEED, 0 };

        enemies[i]->hitbox = (Rectangle){
            positions[i].x,
            positions[i].y,
            tile_size*scale,
            tile_size*scale
        };

    }

    return enemies;

}

void handleEnemyMovement(Enemy* enemy, Map* map, float delta_time){

    enemy->velocity.y = GRAVITY;

    Vector2 collision_point = { 0, 0 };
	Vector2 collision_normal = { 0, 0 };
	float contact_time = 0;

    for (int i = 0; i < map->collisions_quantity; i++){

        if (checkEnemyCollision(enemy, &map->collisions[i], &collision_point, &collision_normal, &contact_time, delta_time)){

            // counter velocity on each axis
            enemy->velocity.x += collision_normal.x * fabs(enemy->velocity.x) * (1-contact_time);
            enemy->velocity.y += collision_normal.y * fabs(enemy->velocity.y) * (1-contact_time);

            

            if ((enemy->pos.x + enemy->hitbox.width + (enemy->velocity.x * delta_time) >= (map->collisions[i].x + map->collisions[i].width)) || (enemy->pos.x + (enemy->velocity.x * delta_time) <= map->collisions[i].x)){
                enemy->velocity.x *= -1;
            } 

        }

    }

    enemy->pos.x += enemy->velocity.x * delta_time;
	enemy->pos.y += enemy->velocity.y * delta_time;

    enemy->hitbox = (Rectangle){enemy->pos.x, enemy->pos.y, enemy->size, enemy->size};

}
