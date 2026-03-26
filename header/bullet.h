#pragma once
#include "gameobject.h"

class Bullet : public GameObject {
public:
	float speed;
	Bullet(float ox, float oy);
    void update(float& dt);
};