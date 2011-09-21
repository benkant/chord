#include <stdlib.h>
#include <stdio.h>

#include "chord_types.h"
#include "ring.h"
#include "util.h"

/**
 * @TODO:
 * check all functions are called
 * check memory leaks
 * "accept list of peers and build a chord ring for these peers"
 * "print chord topology figure showing node ID locations (see L5 S10)"
 * "print the content of any node in the ring"
 * "insert and retrieve documents into/from the network:"
 *   "a list of documents will be given to the program, and the program should be able
 *   to allocate these documents to nodes using chord strategies."
 *   "Design your own hash functions. You may use file names as the input for hashing."
 * "accept a text query in terms of file name and reply with:"
 *   "Print all nodes containing answer and the adopted route for transferring the data" or
 *   "Print a message indicating a query answer is not found"
 * 
 * "You need to demonstrate both code and simulation results. Your report should have enough 
 * sample simulation results to demonstrate that you have met all required specifications and features."
 *
 * Part III: Reliability
 * "your program should allow a random number of nodes in joining in, departing, failure etc"
 */

char* random_string(int length);
void do_main_menu();
void do_node_add();
void do_node_print();
void do_node_leave();
void do_node_fail();
Node *do_node_get();
void do_document_add();
void do_document_query();
void do_ring_print();

int main(int argc, char *argv[]) {
  /*
  const int NUM_NODES = 10;
  int i;
  char *node_id;
  */

  printf("Starting...\n");

  /* create some random nodes */
  /* @TODO: check for duplicate node id's */
  /*
  for (i = 0; i < NUM_NODES; i++) {
    node_id = random_string(NODE_ID_LENGTH);
    ring_create_node(node_id);
  }

  ring_print(FALSE);
  */

  do_main_menu();

  return EXIT_SUCCESS;
}

void do_main_menu() {
  char *prompt = "> ";
  int exit = FALSE;
  int option;

  while (!exit) {
    printf("\nMain Menu:\n");
    printf("1) Add node\n");
    printf("2) Add document\n");
    printf("3) Query document\n");
    printf("4) Print ring\n");
    printf("5) Print node\n");
    printf("6) Node leave\n");
    printf("7) Node fail\n");
    printf("8) Exit\n\n");

    getInteger(&option, MAX_OPTION_INPUT_LENGTH, prompt, OPTION_MIN, OPTION_MAX);  

    switch (option) {
      case 1:
        do_node_add();
        break;
      case 2:
        do_document_add();
        break;
      case 3:
        do_document_query();
        break;
      case 4:
        do_ring_print();
        break;
      case 5:
        do_node_print();
        break;
      case 6:
        do_node_leave();
        break;
      case 7:
        do_node_fail();
        break;
      case 8:
        exit = TRUE;
    }

    option = 0;
  }
}

Node *do_node_get() {
  char *prompt = "Select a node: ";
  Node *node = NULL;
  int node_idx = -100;
  int node_idx_max = ring_size();

  if (node_idx_max != 0) {
    do {
      ring_print(TRUE);

      getInteger(&node_idx, MAX_NODE_IDX, prompt, NODE_IDX_MIN, node_idx_max);

      if (node_idx == RETURN_TO_MENU) {
        break;
      }

      node = ring_get_node(node_idx);

    } while (node == NULL);
  }
  else {
    D1("Ring is empty");
  }

  return node;
}

void do_node_add() {
  char *prompt = "Enter node ID in hex, max 10 chars: ";
  char *new_node_id;
  Node *new_node, *existing_node;

  if ((new_node_id = malloc(sizeof(char) * NODE_ID_LENGTH)) == NULL) {
    BAIL("Failed to create node ID string");
  }

  getString(new_node_id, NODE_ID_LENGTH, prompt);

  if (ring_size() == 0) {
    /* first node added to ring */
    new_node = node_init(new_node_id, TRUE);
    
    existing_node = new_node;
  }
  else {
    new_node = node_init(new_node_id, FALSE);

    existing_node = do_node_get();
  }

  if (existing_node == NULL) {
    D1("Could not get node");
  }
  else {
    D2("Joining to node", existing_node->id);
    node_join(existing_node, new_node);
    node_stabilise(new_node);
  }
}

void do_node_print() {
  Node *node = do_node_get();

  node_print(node);
  node_print_finger_table(node);
}

void do_node_leave() {
}

void do_node_fail() {
}

void do_document_add() {
}

void do_document_query() {
}

void do_ring_print() {
  ring_print(FALSE);
}

char* random_string(int length) {
  char *random;
  int i, c;
  char *chars = "abcdef0123456789";

  /* allocate mem for random string */
  if ((random = malloc(sizeof(char) * length)) == NULL) {
    BAIL("Failed to allocate memory for random string");
  }

  for (i = 0; i < length; i++) {
    c = (int) (rand() % strlen(chars));
    random[i] = chars[c];
  }

  return random;
}
