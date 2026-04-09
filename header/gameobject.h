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

static class World {
public:
    static std::vector<std::unique_ptr<GameObject>> objects;
    static std::vector<GameObject*> colliders;

    void update(float& dt);
    void draw();
}