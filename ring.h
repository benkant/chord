#ifndef _RING_H
#define _RING_H

#include "chord_types.h"

void ring_get_node(int idx);
void ring_create_node(int node_id);
int ring_size();
int ring_key_max();

#endif
