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

int main(int argc, char *argv[]) {
  const int NUM_NODES = 10;
  int i;
  char *node_id;
  char new_node_id[NODE_ID_LENGTH];
  char *prompt = "Enter a node ID: ";

  printf("Starting...\n");

  /* create some random nodes */
  /* @TODO: check for duplicate node id's */
  for (i = 0; i < NUM_NODES; i++) {
    node_id = random_string(NODE_ID_LENGTH);
    ring_create_node(node_id);
  }

  ring_print();

  while (getString(new_node_id, NODE_ID_LENGTH, prompt) != RETURN_TO_MENU) {
    printf("New node %s coming up\n", new_node_id);
  }

  return EXIT_SUCCESS;
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
