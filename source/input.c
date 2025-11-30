#include "input.h"

void get_input(input *inp)
{
	hidScanInput();
    inp->down = hidKeysDown();
    inp->up = hidKeysUp();
    inp->held = hidKeysHeld();
    hidCircleRead(&inp->stick);
}