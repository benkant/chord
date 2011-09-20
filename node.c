#include "node.h"

Node* node_init(char *id) {
  Node *node;

  if ((node = malloc(sizeof(Node))) == NULL) {
    BAIL("Failed to allocate memory for Node");
  }

  node->id = id;
  node->key = chord_hash(id);
  node->finger_table = finger_table_init(node);
  node_create(node);

  return node;
}

Node* find_successor(int key) {
  Node *successor;

  return successor;
}

Node* closest_preceding_node(int key) {
  Node *predecessor;

  return predecessor;
}

void node_create(Node *node) {
}

void node_join(Node *node) {
}

void node_stabilise(Node *node) {
}

void node_notify(Node *node) {
}

void node_fix_fingers(Node *node) {
}

void node_check_predecessor(Node *node) {
}

