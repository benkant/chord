#include <math.h>
#include "ring.h"

static Ring *ring;

Node* ring_get_node(int idx) {
  Node *current;
  int i = 0;

  current = ring->first_node;
  while (current != NULL) {
    i++;
    if (idx == i) {
      return current;
    }
    current = current->successor != current ? current->successor : NULL;
  }

  return current;
}

void ring_create_node(char *node_id) {
  Node *node;
  /* @TODO */
  printf("Shouldn't be here\n");
 
  D2("Creating Node:", node_id);

  node = node_init(node_id, FALSE);

  /* @TODO: check node_id doesn't exist */

  ring_insert(node);
}

void ring_insert(Node *node) {
  Node *current;
  Ring *ring = ring_get();

  if (ring->first_node == NULL) {
    ring->first_node = node;
    ring->last_node = node;

    node->predecessor = NULL;
    node->successor = NULL;
  }
  else {
    current = ring->first_node;
    /* insert the node into the doubly linked list
     * while maintaining sort order. First get the
     * node we will insert after.
     */
    while (current != NULL) {
      /* if there's no successor we're at the end, so add it here */
      if (current->successor == NULL) {
        if (node->key > current->key) {
          ring_insert_after(current, node);
        }
        else {
          ring_insert_before(current, node);
        }
        break;
      }
      else {
        /* if the successor node is less than the current, insert here */
        if ((node->key < current->successor->key) && (node->key > current->key)) {
          /* insert the node into the Ring */
          ring_insert_after(current, node);
          break;
        }
        /* if the node is less and current and greater than predecessor, insert here */
        else if ((node->key < current->key) 
            && ((current->predecessor == NULL) 
            || (node->key > current->predecessor->key))) {
          /* insert the node into the Ring */
          ring_insert_before(current, node);
          break;
        }
      }

      /* go to the next node in the ring */
      current = current->successor != current ? current->successor : NULL;
    }
  }
}

void ring_insert_before(Node *before_node, Node *node) {
  Ring *ring = ring_get();

  node->predecessor = before_node->predecessor;
  node->successor = before_node;
  if (before_node->predecessor == NULL) {
    ring->first_node = node;
  }
  else {
    before_node->predecessor->successor = node;
  }
  before_node->predecessor = node;
}

void ring_insert_after(Node *after_node, Node *node) {
  Ring *ring = ring_get();

  node->predecessor = after_node;
  node->successor = after_node->successor;
  if (after_node->successor == NULL) {
    ring->last_node = node;
  }
  else {
    after_node->successor->predecessor = node;
  }
  after_node->successor = node;
}

int ring_size() {
  Node *current;
  Ring *ring = ring_get();
  int size = 0;

  current = ring->first_node;
  
  while (current != NULL) {
    size++;
    current = current->successor != current ? current->successor : NULL;
  }

  return size;
}

int ring_key_max() {
  return pow(2, KEY_BITS);
}

void ring_print(int index) {
  Node *current;
  Ring *ring = ring_get();
  int i = 0;
 
  current = ring->first_node;

  while (current != NULL) {
    i++;
    if (index) {
      printf("%d) ", i);
    }
    node_print(current);
    current = current->successor != current ? current->successor : NULL;
  }
}

Ring *ring_get() {
  if (ring == NULL) {
    D1("Ring is NULL, creating");
    
    if ((ring = malloc(sizeof(Ring))) == NULL) {
      BAIL("Failed to allocate memory for Ring");
    }

    ring->size = 0;
    ring->first_node = NULL;
  }

  return ring;
}
