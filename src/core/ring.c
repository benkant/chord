#include <math.h>
#include "ring.h"

static Ring *g_ring;
static Node **g_nodes;
static int g_num_nodes = 0;

Node* ring_get_node(int idx) {
  Node *current = NULL;
  int i = 0;
  
  current = g_ring->first_node;
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
  Node *node = NULL;
  /* @TODO */
  printf("Shouldn't be here\n");
  
  D2("Creating Node:", node_id);
  
  node = node_init(node_id);
  
  /* @TODO: check node_id doesn't exist */
  
  ring_insert(node);
}

void ring_insert(Node *node) {
  Node *current = NULL;
  Ring *r = ring_get();
  
  if (r->first_node == NULL) {
    r->first_node = node;
    r->last_node = node;
    
    node->predecessor = NULL;
    node->successor = NULL;
  }
  else {
    current = r->first_node;
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
      
      /* go to the successor node in the ring */
      current = current->successor != current ? current->successor : NULL;
    }
  }
}

void ring_insert_before(Node *before_node, Node *node) {
  Ring *r = ring_get();
  
  node->predecessor = before_node->predecessor;
  node->successor = before_node;
  if (before_node->predecessor == NULL) {
    r->first_node = node;
  }
  else {
    before_node->predecessor->successor = node;
  }
  before_node->predecessor = node;
}

void ring_insert_after(Node *after_node, Node *node) {
  Ring *r = ring_get();
  
  node->predecessor = after_node;
  node->successor = after_node->successor;
  if (after_node->successor == NULL) {
    r->last_node = node;
  }
  else {
    after_node->successor->predecessor = node;
  }
  after_node->successor = node;
}

int ring_size() {
  Ring *r = ring_get();
  return (int)r->size;
}

int ring_key_max() {
  return (int)pow(2, KEY_BITS);
}

void ring_add(Node *node) {
  g_num_nodes++;
  
  if ((g_nodes = realloc(g_nodes, sizeof(void*) * (size_t)g_num_nodes)) == NULL) {
    BAIL("Failed to realloc rings array");
  }
      
  g_nodes[g_num_nodes] = node;
}

void ring_stabilise_all() {
  Ring *r = ring_get();
  
  for (unsigned int i = 0; i < r->size; i++) {
    Node *current = r->nodes[i];
    node_stabilise(current);
    node_fix_fingers(current);
    
    /*
    current = current->successor != current ? current->successor : NULL;
    
    if (done_first && current == ring->first_node) {
      current = NULL;
    }
    
    if (!done_first) {
      done_first = 1;
    }
     */
  }
}

void ring_print(int index, int with_fingers) {
  Ring *r = ring_get();
  int i = 0;
  int done_first = 0;
  
  Node *current = r->first_node;
  
  if (index) {
    printf("%-4s %-4s %-11s %-5s %-5s %-7s\n", "Idx", "Key", "ID", "Pred", "Succ", "# Docs");
    printf("---- ---- ----------- ----- ----- -------\n");
  }
  if (!index) {
    printf("%-4s %-11s %-5s %-5s %-7s\n", "Key", "ID", "Pred", "Succ", "# Docs");
    printf("---- ----------- ----- ----- -------\n");
  }
  
  while (current != NULL) {
    i++;
   
    if (index) {
      printf("%-4d ", i);
    }
    node_print(current);
    if (with_fingers) {
      node_print_finger_table(current);
    }
      
    current = current->successor != current ? current->successor : NULL;
    
    if (done_first && current == r->first_node) {
      current = NULL;
    }
    
    if (!done_first) {
      done_first = 1;
    }
    
  }
}

void ring_print_all(int index, int with_fingers) {
  Ring *r = ring_get();

  if (index) {
    printf("%-4s %-4s %-11s %-5s %-5s %-7s\n", "Idx", "Key", "ID", "Pred", "Succ", "# Docs");
    printf("---- ---- ----------- ----- ----- -------\n");
  }
  if (!index) {
    printf("%-4s %-11s %-5s %-5s %-7s\n", "Key", "ID", "Pred", "Succ", "# Docs");
    printf("---- ----------- ----- ----- -------\n");
  }

  for (unsigned int i = 0; i < r->size; i++) {
    Node *current = r->nodes[i];

    if (index) {
      printf("%-4d ", (int)(i + 1));
    }
    node_print(current);
    if (with_fingers) {
      node_print_finger_table(current);
    }
  }
}


Ring *ring_get() {
  if (g_ring == NULL) {
    D1("Ring is NULL, creating");
    
    if ((g_ring = malloc(sizeof(Ring))) == NULL) {
      BAIL("Failed to allocate memory for Ring");
    }
    
    g_ring->size = 0;
    g_ring->first_node = NULL;
  }
  
  return g_ring;
}
