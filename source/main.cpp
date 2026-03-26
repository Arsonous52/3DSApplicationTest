#include <citro2d.h>
#include <3ds.h>
#include <vector>
#include <fstream>
#include <memory>

#include "input.h"
#include "ship.h"
#include "bullet.h"

typedef struct {
    u8* data;
    u32 size;
    u32 sampleRate;
} Sound;
Input input;

C3D_RenderTarget* topScreen;
C3D_RenderTarget* bottomScreen;
ndspWaveBuf waveBuf;

bool terminateProgram = false;
float gametime = 0;

void load();
void update(float& dt);
void draw(C3D_RenderTarget* screen);
void exit();

// set up basic behaviours
void get_input(Input& input) {
	hidScanInput();
    input.down = hidKeysDown();
    input.up = hidKeysUp();
    input.held = hidKeysHeld();
    hidCircleRead(&input.stick);
}
Sound load_wav(const char* path) {
    Sound sound{};
    FILE* f = fopen(path, "rb");
    if (!f) return sound;

    char chunkId[4];
    u32 chunkSize;

    // Skip RIFF header
    fseek(f, 12, SEEK_SET);

    while (true) {
        if (fread(chunkId, 1, 4, f) != 4) break;
        fread(&chunkSize, 4, 1, f);

        if (memcmp(chunkId, "fmt ", 4) == 0) {
            u16 audioFormat;
            u16 numChannels;
            u32 sampleRate;
            u16 bitsPerSample;

            fread(&audioFormat, 2, 1, f);
            fread(&numChannels, 2, 1, f);
            fread(&sampleRate, 4, 1, f);
            fseek(f, 6, SEEK_CUR); // byte rate + block align
            fread(&bitsPerSample, 2, 1, f);

            if (audioFormat != 1 || bitsPerSample != 16 || numChannels != 2) {
                fclose(f);
                return sound; // unsupported format
            }
            sound.sampleRate = sampleRate;
            fseek(f, chunkSize - 16, SEEK_CUR);
        }
        else if (memcmp(chunkId, "data", 4) == 0) {
            sound.size = chunkSize;
            sound.data = (u8*)linearAlloc(sound.size);

            fread(sound.data, 1, sound.size, f);
            fclose(f);
            return sound;
        }
        else fseek(f, chunkSize, SEEK_CUR);
    }
    fclose(f);
	if (!sound.data || sound.size == 0) {
		svcBreak(USERBREAK_PANIC);
	}
    return sound;
}
void play_sound(Sound* sound) {
    memset(&waveBuf, 0, sizeof(ndspWaveBuf));

    waveBuf.data_vaddr = sound->data;
    waveBuf.nsamples = sound->size / 4; // stereo PCM16

    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
    ndspChnSetRate(0, sound->sampleRate);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    ndspChnWaveBufAdd(0, &waveBuf);
}
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
	load();
	while (aptMainLoop()) {
		get_input(input);

        u64 currentTick = svcGetSystemTick();
        float dt = (float)(currentTick - lastTick) / SYSCLOCK_ARM9;
        lastTick = currentTick;
		gametime += dt;

		update(dt);

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		draw(topScreen);
		C2D_ViewTranslate(-40, -240);
		draw(bottomScreen);
		C2D_ViewReset();
		C3D_FrameEnd(0);

		if (terminateProgram) break; // Ends the game and returns to hbmenu
	}
	exit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	ndspExit();
	return 0;
}

// game logic
u32 white;
u32 black;
u32 grey;

Player* player = nullptr;
std::vector<std::unique_ptr<GameObject>> objects;

std::string gametimeStr;
C2D_TextBuf buf;
C2D_Text text;
C2D_Font font;

float shotCool = 0;
Sound shoot;

void load() {
	white = C2D_Color32(255, 255, 255, 255);
	black = C2D_Color32(0, 0, 0, 255);
	grey = C2D_Color32(100, 100, 100, 255);

    auto p = std::make_unique<Player>();
    player = p.get();
    objects.push_back(std::move(p));
	objects.push_back(std::make_unique<Enemy>());

	buf = C2D_TextBufNew(4096);
	font = C2D_FontLoad("romfs:/monospace.bcfnt");
	if (!font) svcBreak(USERBREAK_PANIC);

	shoot = load_wav("romfs:/data/shoot.wav");
}

void update(float& dt) {
	if (input.down & KEY_START) terminateProgram = true; // Terminate program

	if (input.held & KEY_A && shotCool <= 0) {
		shotCool = 0.2;
		objects.push_back(std::make_unique<Bullet>(player->x, player->y));
		play_sound(&shoot);
	}
	shotCool -= dt;

	for (const auto& object : objects) object->update(dt);
    
    gametimeStr = std::to_string(gametime);
    C2D_TextBufClear(buf);
    C2D_TextFontParseLine(&text, font, buf, gametimeStr.c_str(), 1);
    C2D_TextOptimize(&text);
}

void draw(C3D_RenderTarget* screen) {
	C2D_TargetClear(screen, grey);
	C2D_SceneBegin(screen);

	for (const auto& object : objects) object->draw();

	C2D_DrawText(&text, C2D_WithColor, 0, 0, 0, 1.0f, 1.0f, white);
}

void exit() {
	linearFree(shoot.data);
}