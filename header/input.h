#pragma once
#include <3ds.h>

typedef struct {
	u32 down;
	u32 up;
	u32 held;
	circlePosition stick;
} Input;

void get_input(Input& input) {
	hidScanInput();
    input.down = hidKeysDown();
    input.up = hidKeysUp();
    input.held = hidKeysHeld();
    hidCircleRead(&input.stick);
}