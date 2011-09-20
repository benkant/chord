#include <stdlib.h>
#include <stdio.h>

#include "chord_types.h"
#include "ring.h"

char* random_string(int length);

int main(int argc, char *argv[]) {
  const int NUM_NODES = 10;
  const int NODE_ID_LENGTH = 10;
  int i;
  char *node_id;

  printf("Starting...\n");

  /* create some random nodes */
  /* @TODO: check for duplicate node id's */
  for (i = 0; i < NUM_NODES; i++) {
    node_id = random_string(NODE_ID_LENGTH);
    ring_create_node(node_id);
  }

  ring_print();

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
