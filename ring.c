#include <math.h>
#include "ring.h"

static Ring *ring;

void ring_get_node(int idx) {

}

void ring_create_node(char *node_id) {
  Node *node;

  if (ring == NULL) {
    DEBUG("Ring is NULL, creating");
    
    if ((ring = malloc(sizeof(Ring))) == NULL) {
      BAIL("Failed to allocate memory for Ring");
    }

    ring->size = 0;
    ring->first_node = NULL;
  }

  DEBUG("Creating Node:"); DEBUG(node_id);

  node = node_init(node_id);

  /* @TODO: check node_id doesn't exist */

  ring_insert(node);
}

void ring_insert(Node *node) {
  Node *current, *after_node;
  if (ring->first_node == NULL) {
    DEBUG("Adding first node");
    node->predecessor = NULL;
    node->successor = NULL;
    ring->first_node = node;
    ring->last_node = NULL;
  }
  else {
    DEBUG("Adding subsequent node");

    current = ring->first_node;
    /* insert the node into the doubly linked list
     * while maintaining sort order. First get the
     * node we will insert after.
     */
    while (current != NULL) {
      /* if there's no successor we're at the end, so add it here */
      if (current->successor == NULL) {
        after_node = current;
      }
      /* if the successor node is less than the current, insert here */
      else if ((current->successor->key < node->key)
          && (current->key > node->key)) {
        after_node = current;
      }
      /* go to the next node in the ring */
      current = current->successor;
    }

    /* insert the node into the Ring */
    node->predecessor = after_node;
    node->successor = after_node->successor;
    if (node->successor == NULL) {
      ring->last_node = node;
    }
    else {
      after_node->successor->predecessor = node;
    }
    after_node->successor = node;
  }
}

int ring_size() {
  return 1;
}

int ring_key_max() {
  return pow(2, KEY_BITS);
}
