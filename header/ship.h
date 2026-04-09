#pragma once
#include <citro2d.h>

#include "input.h"
#include "gameobject.h"

class Ship : public GameObject {
public:
	int health;
	float speed;
	float shotCool;
	C2D_SpriteSheet sheet;
	C2D_Sprite sprite;
	Ship();
};

class Enemy : public Ship {
public:
	Enemy();
	void update(float& dt);
};

class Player : public Ship {
public:
	Player();
	void update(float& dt);
	//void draw();
};