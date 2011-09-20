#include "node.h"

Node* node_init(char *id) {
  Node *node;

  if ((node = malloc(sizeof(Node))) == NULL) {
    BAIL("Failed to allocate memory for Node");
  }

  node->id = id;
  node->key = chord_hash(id);
  node->finger_table = finger_table_init(node);
  node->state = NODE_STATE_RUNNING;
  node_create(node);

  return node;
}

Node* node_find_successor(Node *node, int key) {
  Node *closest_preceding_node;
  if (node->successor->key == key) {
    return node->successor;
  }
  else {
    closest_preceding_node = node_closest_preceding_node(node, key);
    return node_find_successor(closest_preceding_node, key);
  }
}

Node* node_closest_preceding_node(Node *node, int key) {
  int i;
  Finger *finger;

  for (i = KEY_BITS; i > 0; i--) {
    finger = node->finger_table->fingers[i];
    if (key_is_between(finger->node->key, key, node->key)) {
      return finger->node;
    }
  }

  return node;
}

void node_create(Node *node) {
  node->predecessor = NULL;
  node->successor = node;
}

void node_join(Node *existing_node, Node *new_node) {
  new_node->predecessor = NULL;
  new_node->successor = node_find_successor(existing_node, new_node->key);
}

void node_stabilise(Node *node) {
  Node *x = node->successor->predecessor;

  if (x != NULL) {
    if (node == node->successor) {
      node->successor = x;
    }
  }
  node_notify(node->successor, node);
}

void node_notify(Node *notify_node, Node *check_node) {
  if ((notify_node->predecessor == NULL)
      || key_is_between(check_node->key, notify_node->predecessor->key, notify_node->key)) {
    notify_node->predecessor = check_node;
  }
}

void node_fix_fingers(Node *node) {
}

void node_check_predecessor(Node *node) {
}

void node_print(Node *node) {
  printf("Key: %d, ID: %s\n", node->key, node->id);
}
