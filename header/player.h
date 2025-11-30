#pragma once
#include "input.h"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

#define GRAVITY_SCALE 100

typedef struct {
	int health;
	float x, y;
	float gravity;
} player;

player player_init(void);

void player_update(player *p, input *inp, float dt);