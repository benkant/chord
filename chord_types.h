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
  int state;
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
#define RETURN_TO_MENU -1
#define TEMP_STRING_LENGTH 1000
#define MAX_OPTION_INPUT_LENGTH 1
#define OPTION_MIN 1
#define OPTION_MAX 8
#define MAX_NODE_IDX 3
#define NODE_IDX_MIN 1

#define NODE_ID_LENGTH 10
#define NODE_STATE_RUNNING 1
#define NODE_STATE_DEAD 2

#ifndef DEBUG_ON
#define DEBUG_ON 0
#endif

/* helpers */
#define BAIL(x) \
  fprintf(stderr, "FATAL: %s\n", x); \
  exit(EXIT_FAILURE);

#define D1(x) \
  if (DEBUG_ON) { \
    printf("DEBUG: %s\n", x); \
  }

#define D2(x, y) \
  if (DEBUG_ON) { \
    printf("DEBUG: %s %s\n", x, y); \
  }

/* warning, contains side effects when used with prefix or postfix increment operators */
#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))

#endif
