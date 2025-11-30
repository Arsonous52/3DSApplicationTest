#include <citro2d.h>
#include <3ds.h>
#include <math.h>

#include "input.h"
#include "player.h"

typedef struct {
	bool terminate;
	float time;
	player ply;
	input inp;
} Game;

Game game = {
	.terminate = false,
	.time = 0,
};

void load()
{
	game.ply = player_init();
}
u32 red;
u32 white;

void update(float dt)
{
	if (game.inp.down & KEY_START) { game.terminate = true; } // Terminate program

	player_update(&game.ply, &game.inp, dt);
}

void draw(C3D_RenderTarget* screen)
{
	C2D_TargetClear(screen, white);
	C2D_SceneBegin(screen);
	
	C2D_DrawRectSolid(game.ply.x, game.ply.y, 0, 50, 50, red);
}

int main(int argc, char* argv[]) {
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Top & Bottom Screen
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	consoleInit(GFX_BOTTOM, NULL);

	white = C2D_Color32(255, 255, 255, 255);
	red = C2D_Color32(255, 0, 0, 255);
	
	printf("\x1b[2;4HMove the player cube with the dpad.\n");
	printf("\x1b[3;15HJump with A.\n");
	printf("\x1b[29;11HPress Start to exit.\n");
    u64 lastTick = svcGetSystemTick();
	load();
	while (aptMainLoop())
	{
		get_input(&game.inp);

        u64 now = svcGetSystemTick();
        float dt = (float)(now - lastTick) / (float)SYSCLOCK_ARM9;
        lastTick = now;
		game.time += dt;

		update(dt);

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		draw(top);
		C3D_FrameEnd(0);

		if (game.terminate) { break; }
	}
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}