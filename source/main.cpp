#include <stdio.h>
#include <3ds.h>

int main()
{
	hidInit();
	gfxInitDefault();
    PrintConsole top, bottom;

    consoleInit(GFX_TOP, &top);
    consoleInit(GFX_BOTTOM, &bottom);

	consoleSelect(&bottom);
	printf("Press 'Start' to exit.");

    consoleSelect(&top);

	while (aptMainLoop())
	{
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) { break; }
		
		printf("\x1b[0;0HThis is a test application.\n");

		time_t unixTime = time(NULL);
		printf("\x1b[30;0H%lld", unixTime);

		gfxFlushBuffers();
		gfxSwapBuffers();
		//gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}