#include <raylib.h>
#include "game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "resource_dir.h"

Map *read_map(char* map_path, int tile_size){

    Map* map = malloc(sizeof(Map));

    SearchAndSetResourceDir("resources");

    FILE* fptr = fopen(map_path, "r");

    char character;
    int cols = 0;
    map->resolution = (Vector2){ 0, 1 };

    while ((character = fgetc(fptr)) != EOF)
    {
        
        if (cols > map->resolution.x){
            map->resolution.x = cols;
        }

        if (character == '\n'){
            map->resolution.y++;
            cols = 0;
        }else{
            cols++;
        }

    }

    fclose(fptr);

    // Map tiles
    map->tiles = (char**)malloc(map->resolution.y * sizeof(char*));

    // failed to allocate memory
    if (map == NULL) return NULL;

    // allocate memory for rows
    for (int i = 0; i < map->resolution.x; i++){
        map->tiles[i] = (char*)malloc(map->resolution.x * sizeof(char));
        if (map == NULL) return NULL;
    }

    fptr = fopen(map_path, "r");

    Vector2 margin = {(GetRenderWidth() - map->resolution.x * tile_size)/2, (GetRenderHeight() - map->resolution.y * tile_size)/2};

    //(SCREEN_HEIGHT - map->resolution.y * tile_size)/2

    for (int y = 0; y < map->resolution.y; y++){
        for (int x = 0; x < map->resolution.x; x++){

            character = fgetc(fptr);

            while (character == '\n' || character == '\r')
            {
                character = fgetc(fptr);
            }
            
            if (character == 'p') map->spawn_pos = tileToPosition((Vector2){ x, y }, margin, tile_size);

            if (character == 'f') map->porta_pos = tileToPosition((Vector2){ x, y }, margin, tile_size);

            map->tiles[y][x] = character;

        }
    }     

    fclose(fptr);

    
    int collision_size = 0;
    int collision_count = 0;

    int ladder_size = 0;
    int ladder_count = 0;

    const int max_rects = ((int)(map->resolution.x * map->resolution.y) % 2) ? (map->resolution.x * map->resolution.y) / 2 + 1 : (map->resolution.x * map->resolution.y) / 2;

    
    Vector2* collisions_positions = (Vector2*)malloc(max_rects * sizeof(Vector2));
    Vector2* collisions_sizes = (Vector2*)malloc(max_rects * sizeof(Vector2));

    Vector2* ladders_positions = (Vector2*)malloc(max_rects * sizeof(Vector2));
    Vector2* ladders_sizes = (Vector2*)malloc(max_rects * sizeof(Vector2));

    for (int y = 0; y < map->resolution.y; y++){
        for (int x = 0; x < map->resolution.x; x++){

            if (map->tiles[y][x] == 'z') collision_size++;
            

            // complete the rect if there isnt a 'z' in the next position
            if (collision_size != 0 && ((x == map->resolution.x - 1 && map->tiles[y][x] == 'z') || (x < map->resolution.x - 1 && map->tiles[y][x+1] != 'z'))){   

                collisions_positions[collision_count].x = (x - collision_size + 1);
                collisions_positions[collision_count].y = y;
                collisions_sizes[collision_count].x = collision_size;
                collisions_sizes[collision_count].y = 1;

                collision_count++;
                collision_size = 0;

            }
        }
    }

    for (int x = 0; x < map->resolution.x; x++){
        for (int y = 0; y < map->resolution.y; y++){     
            
            int is_ladder = (map->tiles[y][x] == 's' || map->tiles[y][x] == 'h' || map->tiles[y][x] == 'd');

            if (is_ladder) ladder_size++;

            if (ladder_size != 0 && (y == map->resolution.y - 1 && is_ladder || (x < map->resolution.y - 1 && (map->tiles[y+1][x] != 's' && map->tiles[y+1][x] != 'h' && map->tiles[y+1][x] != 'd')))){

                ladders_positions[ladder_count].x = x;
                ladders_positions[ladder_count].y = (y - ladder_size + 1);
                ladders_sizes[ladder_count].x = 1;
                ladders_sizes[ladder_count].y = ladder_size;

                ladder_count++;
                ladder_size = 0;

            }
        }
    }

    map->collisions_quantity = collision_count;

    map->collisions = (Rectangle*)malloc(map->collisions_quantity * sizeof(Rectangle));

    for (int i = 0; i < map->collisions_quantity; i++){

        Vector2 position = tileToPosition(collisions_positions[i], margin, tile_size);

        map->collisions[i] = (Rectangle){

            (float)position.x,
            (float)position.y,
            (float)collisions_sizes[i].x * tile_size,
            (float)collisions_sizes[i].y * tile_size
        };
            
    }

    map->ladders_quantity = ladder_count;

    map->ladders = (Rectangle*)malloc(map->ladders_quantity * sizeof(Rectangle));

    for (int i = 0; i < map->ladders_quantity; i++){

        Vector2 position = tileToPosition(ladders_positions[i], margin, tile_size);

        map->ladders[i] = (Rectangle){
            (float)position.x,
            (float)position.y,
            (float)ladders_sizes[i].x * tile_size,
            (float)ladders_sizes[i].y * tile_size
        };

    }

    free(collisions_positions);
    free(collisions_sizes);

    return map;
}