#include <citro2d.h>
#include <3ds.h>
#include <vector>
#include <fstream>
#include <memory>

#include "input.h"
#include "sound.h"
#include "ship.h"
#include "bullet.h"

Input input;

C3D_RenderTarget* topScreen;
C3D_RenderTarget* bottomScreen;

float gametime = 0;

void load();
void update(float& dt);
void draw(C3D_RenderTarget* screen);

// should always run on exit - required cleanup
void terminateProgram() {
	linearFree(shoot.data);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    ndspExit();
    svcExitProcess(); // this is what actually exits the program
}

// set up basic behaviours
int main(int argc, char* argv[]) {
	gfxInitDefault();
	romfsInit();
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspSetMasterVol(1.0f);
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Top & Bottom Screen
	topScreen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    u64 lastTick = svcGetSystemTick();
	u64 currentTick;
	load();
	while (aptMainLoop()) {
		get_input(input);

        currentTick = svcGetSystemTick();
        float dt = (float)(currentTick - lastTick) / SYSCLOCK_ARM9;
		gametime += dt;
        lastTick = currentTick;

		update(dt);

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		draw(topScreen);
		C2D_ViewTranslate(-40, -240); // seamless transition between screens
		draw(bottomScreen);
		C2D_ViewReset();
		C3D_FrameEnd(0);
	}
    terminateProgram();
	return -1; // should never reach here unless termination failed
}

// game logic
u32 white;
u32 black;
u32 grey;

Player* player = nullptr;

char gametimeStr[256];
C2D_TextBuf buf;
C2D_Text text;
C2D_Font font;

Sound shoot;

void load() {
	white = C2D_Color32(255, 255, 255, 255);
	black = C2D_Color32(0, 0, 0, 255);
	grey = C2D_Color32(100, 100, 100, 255);

    auto p = std::make_unique<Player>();
    player = p.get();
    World::objects.push_back(std::move(p));
	World::objects.push_back(std::make_unique<Enemy>());

	buf = C2D_TextBufNew(4096);
	font = C2D_FontLoad("romfs:/monospace.bcfnt");
	if (!font) svcBreak(USERBREAK_PANIC);

	shoot = load_wav("romfs:/data/shoot.wav");
}

void update(float& dt) {
	if (input.down & KEY_START) terminateProgram();
    World::update(dt);
    
	// update the gametime text to be accurate
    sprintf(gametimeStr, sizeof(gametimeStr), "Time: %.2f", gametime);
    C2D_TextBufClear(buf);
    C2D_TextFontParseLine(&text, font, buf, gametimeStr, 1);
    C2D_TextOptimize(&text);
}

void draw(C3D_RenderTarget* screen) {
	C2D_TargetClear(screen, grey);
	C2D_SceneBegin(screen);

    World::draw();
	C2D_DrawText(&text, C2D_WithColor, 0, 0, 0, 1.0f, 1.0f, white);
}