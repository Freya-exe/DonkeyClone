#include <raylib.h>
#include "game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "resource_dir.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

Map *read_map(char* map_path, int tile_size, int scale){

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

    Vector2 margin = {(GetRenderWidth() - map->resolution.x * tile_size * scale)/2, (GetRenderHeight() - map->resolution.y * tile_size * scale)/2};
    
    map->enemies_quantity = 0;
    int max_enemy_count = map->resolution.x * map->resolution.y;
    map->enemies_pos = (Vector2*)malloc(max_enemy_count * sizeof(Vector2));

    for (int y = 0; y < map->resolution.y; y++){
        for (int x = 0; x < map->resolution.x; x++){

            character = fgetc(fptr);

            while (character == '\n' || character == '\r')
            {
                character = fgetc(fptr);
            }
            
            if (character == 'p') map->spawn_pos = tileToPosition((Vector2){ x, y }, margin, tile_size * scale);

            if (character == 'f') map->porta_pos = tileToPosition((Vector2){ x, y }, margin, tile_size * scale);

            if (character == 'e') {
                map->enemies_pos[map->enemies_quantity] = tileToPosition((Vector2){ x, y }, margin, tile_size * scale);
                map->enemies_quantity++;
            }

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

        Vector2 position = tileToPosition(collisions_positions[i], margin, tile_size * scale);

        map->collisions[i] = (Rectangle){
            (float)position.x,
            (float)position.y,
            (float)collisions_sizes[i].x * tile_size * scale,
            (float)collisions_sizes[i].y * tile_size * scale
        };
            
    }

    map->ladders_quantity = ladder_count;

    map->ladders = (Rectangle*)malloc(map->ladders_quantity * sizeof(Rectangle));

    for (int i = 0; i < map->ladders_quantity; i++){

        Vector2 position = tileToPosition(ladders_positions[i], margin, tile_size * scale);

        map->ladders[i] = (Rectangle){
            (float)position.x,
            (float)position.y,
            (float)ladders_sizes[i].x * tile_size * scale,
            (float)ladders_sizes[i].y * tile_size * scale
        };

    }

    free(collisions_positions);
    free(collisions_sizes);

    map->render_texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    createMapTexture(map, margin, tile_size, scale);

    return map;
}

void createMapTexture(Map* map, Vector2 margin, int tile_size, int scale){

    Texture2D tileset = LoadTexture("tileset.png");

    BeginTextureMode(map->render_texture);

    ClearBackground(BLACK);

    for (int y = 0; y < map->resolution.y; y++){
        for (int x = 0; x < map->resolution.x; x++){

            char tile = map->tiles[y][x];
            Vector2 position = tileToPosition((Vector2){x,y}, margin, tile_size * scale);
            Rectangle source_rect = {16, 0, 16, 16};
            Rectangle dest_rect = {position.x, position.y, tile_size * scale, tile_size * scale};
            Vector2 origin = {0,0};

            switch (tile){
                case 'z':
                    source_rect.x = 16;
                    DrawTexturePro(tileset, source_rect, dest_rect, origin, 0, WHITE);
                    break;

                case 'e':
                    DrawRectangle(position.x, position.y, tile_size * scale, tile_size * scale, RED);
                    break;

                case 's':
                    source_rect.x = 0;
                    DrawTexturePro(tileset, source_rect, dest_rect, origin, 0, WHITE);
                    break;

                case 'h':
                    source_rect.x = 0;
                    DrawTexturePro(tileset, source_rect, dest_rect, origin, 0, WHITE);
                    break;
                
                case 'd':
                    source_rect.x = 0;
                    DrawTexturePro(tileset, source_rect, dest_rect, origin, 0, WHITE);
                    break;

                case 'f':
                    DrawRectangle(position.x, position.y, tile_size * scale, tile_size * scale, BROWN);
                    break;
                
                case 'p':
                    DrawRectangle(position.x, position.y, tile_size * scale, tile_size * scale, PINK);
                    break;

                default:
                    break;
            }
        }
    }

    EndTextureMode();


}