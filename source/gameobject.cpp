#include "gameobject.h"

GameObject::GameObject() {
    x = 0; y = 0;
    width = 20; height = 20;
	colour = C2D_Color32(255, 255, 255, 255);
}

GameObject::~GameObject() {
    //free up resources
}

void GameObject::draw() {
	C2D_DrawRectSolid(x, y, 0, width, height, colour);
}