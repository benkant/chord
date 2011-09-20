#ifndef _CHORD_TYPES_H
#define _CHORD_TYPES_H

#include <stdlib.h>
#include <stdio.h>

/* Finger and FingerTable */
typedef struct Finger {
  struct Node *node;
  int start;
} Finger;

typedef struct FingerTable {
  struct Finger **fingers;
  int length;
} FingerTable;

/* Node */
typedef struct Node {
  char *id;
  int key;
  struct Node *predecessor;
  struct Node *successor;
  struct FingerTable *finger_table;
} Node;

/* Chord Ring */
typedef struct Ring {
  Node *first_node;
  Node *last_node;
  unsigned size;
} Ring;

/* defines */
#define KEY_BITS 8
#define TRUE 1
#define FALSE 0

#ifndef DEBUG_ON
#define DEBUG_ON 0
#endif

/* helpers */
#define BAIL(x) \
  fprintf(stderr, "FATAL: %s\n", x); \
  exit(EXIT_FAILURE);

#define DEBUG(x) \
  if (DEBUG_ON) { \
    printf("DEBUG: %s\n", x); \
  }

#endif
