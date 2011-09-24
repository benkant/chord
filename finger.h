#ifndef _FINGER_H
#define _FINGER_H

#include <math.h>
#include "chord_types.h"
#include "key.h"

Finger* finger_init(Node *node, int key);
FingerTable* finger_table_init(Node *node);

#endif
