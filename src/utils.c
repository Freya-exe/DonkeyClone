#include <stdlib.h>
#include "utils.h"

void free_map(Map* map){

    /*
    map->tiles = (char**)malloc(map->resolution.y * sizeof(char*));

    // failed to allocate memory
    if (map == NULL) return NULL;

    // allocate memory for rows
    for (int i = 0; i < map->resolution.x; i++){
        map->tiles[i] = (char*)malloc(map->resolution.x * sizeof(char));
        if (map == NULL) return NULL;
    }
    */

    for (int i = 0; i < map->resolution.x; i++)
    {
        free(map->tiles[i]);
    }

    free(map->tiles);
    free(map->collisions);
    free(map);
    

}

Vector2 tileToPosition(Vector2 coord, Vector2 margin, int tile_size){

    Vector2 position = {margin.x + coord.x * tile_size, margin.y + coord.y * tile_size};

    return position;
}