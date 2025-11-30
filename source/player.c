#include "player.h"

player player_init(void)
{
    return (player){
        .health  = 100,
        .x       = 10,
        .y       = 10,
        .gravity = 0,
    };   
}

void player_update(player *p, input *inp, float dt)
{
	p->x += inp->stick.dx * dt;
	p->y += p->gravity * dt;

	p->gravity += GRAVITY_SCALE * dt;
	if(p->y > SCREEN_HEIGHT - 50)
	
	{
		p->y = SCREEN_HEIGHT - 50;
		p->gravity = 0;
	}
	if(inp->down & KEY_A) { p->gravity = -150; }
}