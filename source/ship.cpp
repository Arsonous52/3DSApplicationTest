#include "ship.h"

extern Input input;

Ship::Ship() {
	health = 100;
	colour = C2D_Color32(255, 255, 255, 255);
}

Enemy::Enemy() {
	speed = 10;
	colour = C2D_Color32(255, 0, 0, 255);
}

void Enemy::update(float& dt) {
	x += speed * dt;
}

Player::Player() {
	x = 50; y = 400;
	speed = 100;
	sheet = C2D_SpriteSheetLoad("romfs:/sprites/playerShip.t3x");
	C2D_SpriteFromSheet(&sprite, sheet, 0);

	colour = C2D_Color32(0, 0, 255, 255);
}

void Player::update(float& dt) {
	x += input.stick.dx * speed * dt /150;
	y += -input.stick.dy * speed * dt /150;

	x = C2D_Clamp(x, 40.0f, 360.0f - width);
}

// void Player::draw() {
//     C2D_SpriteSetPos(&sprite, x, y);
// 	C2D_DrawSprite(&sprite);
// }