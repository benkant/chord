#ifndef _NODE_H
#define _NODE_H

#include "chord_types.h"
#include "hash.h"
#include "finger.h"

Node* node_init(char *id);
Node* node_find_successor(Node *node, int key);
Node* node_find_successor_impl(Node *orig_node, Node *node, int key, int depth);
Node* node_closest_preceding_node(Node *node, int key);
void node_create(Node *node);
void node_join(Node *existing_node, Node *new_node);
void node_stabilise(Node *node);
void node_notify(Node *notify_node, Node *check_node);
void node_fix_fingers(Node *node);
void node_check_predecessor(Node *node);
void node_print(Node *node);
void node_print_documents(Node *node);
void node_print_finger_table(Node *node);
void node_document_add(Node *node, Document *doc);
void node_document_store(Node *node, Document *doc);
void node_document_query(Node *node, char *filename);
Document* node_document_exists(Node *node, char *filename);
void node_document_print(Node *node, Document *doc);

#endif
