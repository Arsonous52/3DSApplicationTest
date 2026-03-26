#include "bullet.h"

Bullet::Bullet(float ox, float oy) {
    x = ox; y = oy;
    width = 5; height = 10;
	colour = C2D_Color32(255, 255, 255, 255);

    speed = 150;
}

void Bullet::update(float& dt) {
	y -= dt * speed;
}