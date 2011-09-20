#ifndef _RING_H
#define _RING_H

#include "chord_types.h"
#include "node.h"

void ring_get_node(int idx);
void ring_create_node(char *node_id);
int ring_size();
int ring_key_max();
void ring_insert(Node *node);

#endif
