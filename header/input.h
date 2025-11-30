#pragma once
#include <3ds.h>

typedef struct {
	u32 down;
	u32 up;
	u32 held;
	circlePosition stick;
} input;

void get_input(input *inp);