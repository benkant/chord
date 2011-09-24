#include "node.h"

Node* node_init(char *id) {
  Node *node = NULL;
  
  if ((node = malloc(sizeof(Node))) == NULL) {
    BAIL("Failed to allocate memory for Node");
  }
  
  node->id = id;
  node->key = chord_hash(id);
  node->finger_table = finger_table_init(node);
  node->state = NODE_STATE_RUNNING;
  
  return node;
}

Node* node_find_successor(Node *node, int key) {
  Node *closest_preceding_node = NULL;
  
  /*if (node->successor->key == key) {*/
  /*if ((key > node->key && key <= node->successor->key)
      || node->successor == node) {*/
  if (key_in_range(key, node->key, node->successor->key, TRUE)
      || node == node->successor) {
    return node->successor;
  }
  else {
    closest_preceding_node = node_closest_preceding_node(node, key);
    if (closest_preceding_node == node) {
      return node_find_successor(node->successor, key);
    }
    return node_find_successor(closest_preceding_node, key);
  }
}

Node* node_closest_preceding_node(Node *node, int key) {
  int i;
  Finger *finger = NULL;
  
  for (i = KEY_BITS - 1; i >= 0; i--) {
    finger = node->finger_table->fingers[i];
    
    /*if (key > node->key && key < finger->start) {*/
    if (key_in_range(finger->start, node->key, key, FALSE)) {
      return finger->node;
    }
  }
  
  return node;
}

void node_create(Node *node) {
  Ring *ring = ring_get();
  
  ring->first_node = node;
  ring->last_node = node;
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
    if (node == node->successor
        || key_in_range(x->key, node->key, node->successor->key, TRUE)) {
      node->successor = x;
    }
  }
  node_notify(node->successor, node);
}

void node_notify(Node *notify_node, Node *check_node) {
  /*
  if ((notify_node->predecessor == NULL)
      || (check_node->key > notify_node->predecessor->key 
          && check_node->key < notify_node->key)) {*/
  if ((notify_node->predecessor == NULL ||
       key_in_range(check_node->key, notify_node->predecessor->key, notify_node->key, FALSE))) {
    
    /* check_node thinks it might be notify_node's predecessor */
    notify_node->predecessor = check_node;
  }
}

void node_fix_fingers(Node *node) {
  int i;
  Finger *finger = NULL;
  
  for (i = 0; i < KEY_BITS; i++) {
    finger = node->finger_table->fingers[i];
    finger->node = node_find_successor(node, finger->start);
  }
}

void node_check_predecessor(Node *node) {
  if (node->predecessor->state == NODE_STATE_DEAD) {
    node->predecessor = NULL;
  }
}

void node_print(Node *node) {
  int predecessor, successor;
  
  predecessor = node->predecessor != NULL ? node->predecessor->key : 0;
  successor = node->successor != NULL ? node->successor->key : 0;
  
  printf("Key: %d, ID: %s\n", node->key, node->id);
  printf("\tPredecessor: %d, successor: %d\n", predecessor, successor);
}

void node_print_finger_table(Node *node) {
  int i;
  Finger *finger = NULL;
  
  for (i = 0; i < KEY_BITS; i++) {
    finger = node->finger_table->fingers[i];
    printf("i: %d, node_id: %s, start: %d\n", i, finger->node->id, finger->start);
  }
}
