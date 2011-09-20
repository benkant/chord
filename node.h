#ifndef _NODE_H
#define _NODE_H

#include "chord_types.h"
#include "hash.h"
#include "finger.h"

Node* node_init(char *id);
Node* node_find_successor(Node *node, int key);
Node* node_closest_preceding_node(Node *node, int key);
void node_create(Node *node);
void node_join(Node *existing_node, Node *new_node);
void node_stabilise(Node *node);
void node_notify(Node *notify_node, Node *check_node);
void node_fix_fingers(Node *node);
void node_check_predecessor(Node *node);
void node_print(Node *node);

#endif
