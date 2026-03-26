#pragma once
#include <3ds.h>
#include <citro2d.h>

class GameObject {
public:
	float x, y, width, height;
	u32 colour;
    GameObject();
    virtual ~GameObject();
    virtual void update(float& dt) = 0;
    virtual void draw();
};