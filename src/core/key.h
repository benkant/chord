#ifndef _KEY_H
#define _KEY_H

#include "chord_types.h"
#include "ring.h"

int key_in_range(int check, int bound1, int bound2, int half);
int key_init(Node *node, int idx);

#endif
