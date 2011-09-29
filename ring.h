#ifndef _RING_H
#define _RING_H

#include "chord_types.h"
#include "node.h"

Node* ring_get_node(int idx);
void ring_create_node(char *node_id);
int ring_size();
int ring_key_max();
void ring_insert(Node *node);
void ring_insert_before(Node *before_node, Node *node);
void ring_insert_after(Node *after_node, Node *node);
void ring_print(int index, int with_fingers);
void ring_print_all(int index, int with_fingers);
Ring* ring_get();
void ring_add(Node *node);
void ring_stabilise_all();

#endif
