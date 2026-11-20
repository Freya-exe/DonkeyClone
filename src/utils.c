#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include <stdio.h>

void free_map(Map* map){

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

bool checkOverlap(Rectangle player, Rectangle target){

	return (player.x < (target.x + target.width) && player.x > (target.x - player.width) && player.y < (target.y + target.height) && player.y > (target.y - player.height));

}

bool checkRayCollision(Vector2 origin, Vector2 ray_dir, Rectangle target, Vector2* collision_point, Vector2* collision_normal, float* t_near_hit){

	// linear equation for near and far contact times
	Vector2 t_near;
	t_near.x = (target.x - origin.x)/(ray_dir.x - origin.x);
	t_near.y = (target.y - origin.y)/(ray_dir.y - origin.y);
	Vector2 t_far;
	t_far.x = (target.x + target.width - origin.x)/(ray_dir.x - origin.x);
	t_far.y = (target.y + target.height - origin.y)/(ray_dir.y - origin.y);

	// sort closest contact time
	if (t_near.x > t_far.x){
		float aux = t_near.x;
		t_near.x = t_far.x;
		t_far.x = aux;
	}
	if (t_near.y > t_far.y){
		float aux = t_near.y;
		t_near.y = t_far.y;
		t_far.y = aux;
	}

	// no collision
	if ((t_near.x > t_far.y) || (t_near.y > t_far.x)) return false;

	*t_near_hit = fmax(t_near.x, t_near.y);
	float t_far_hit = fmin(t_far.x, t_far.y);

	// collision occurs outside the ray length
	if (t_far_hit < 0) return false;

	collision_point->x = origin.x + (*t_near_hit) * (ray_dir.x - origin.x);
	collision_point->y = origin.y + (*t_near_hit) * (ray_dir.y - origin.y);

	//collision_point->x = origin.x + (*t_near_hit) * (ray_dir.x);
	//collision_point->y = origin.y + (*t_near_hit) * (ray_dir.y);

	if (t_near.x > t_near.y)
		if (ray_dir.x - origin.x < 0) 
			*collision_normal = (Vector2){ 1, 0 };
		else
			*collision_normal = (Vector2){ -1, 0 };
	else if (t_near.x < t_near.y)
		if (ray_dir.y - origin.y < 0)
			*collision_normal = (Vector2){ 0, 1 };
		else
			*collision_normal = (Vector2){ 0, -1 };
	
	return true;

}

bool checkPlayerCollision(Player* player, Rectangle* target, Vector2* collision_point, Vector2* collision_normal, float* contact_time, float delta_time){

	if (player->velocity.x == 0 && player->velocity.y == 0)
	{
		return false;
	}
	
	Rectangle expanded_target;
	expanded_target.x = target->x - player->hitbox.width/2;
	expanded_target.y = target->y - player->hitbox.height/2;
	expanded_target.width = target->width + player->hitbox.width;
	expanded_target.height = target->height + player->hitbox.height;

	Vector2 origin = {player->pos.x + player->hitbox.width/2, player->pos.y + player->hitbox.height/2};
	Vector2 dir = {origin.x + player->velocity.x * delta_time, origin.y + player->velocity.y * delta_time};
	
	if (checkRayCollision(origin, dir, expanded_target, collision_point, collision_normal, contact_time)){
		// contact in range of collision
		if(*contact_time < 1 && *contact_time >= 0){
			return true;
		} 	
	}

	return false;

}

bool checkEnemyCollision(Enemy* enemy, Rectangle* target, Vector2* collision_point, Vector2* collision_normal, float* contact_time, float delta_time){

	if (enemy->velocity.x == 0 && enemy->velocity.y == 0)
	{
		return false;
	}

	Rectangle expanded_target;
	expanded_target.x = target->x - enemy->hitbox.width/2;
	expanded_target.y = target->y - enemy->hitbox.height/2;
	expanded_target.width = target->width + enemy->hitbox.width;
	expanded_target.height = target->height + enemy->hitbox.height;

	Vector2 origin = {enemy->pos.x + enemy->hitbox.width/2, enemy->pos.y + enemy->hitbox.height/2};
	Vector2 dir = {origin.x + enemy->velocity.x * delta_time, origin.y + enemy->velocity.y * delta_time};
	
	if (checkRayCollision(origin, dir, expanded_target, collision_point, collision_normal, contact_time)){
		// contact in range of collision
		if(*contact_time < 1 && *contact_time >= 0){
			return true;
		} 	
	}

	return false;

}