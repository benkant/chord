#ifndef _CHORD_TYPES_H
#define _CHORD_TYPES_H

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
  int id;
  int key;
  struct Node *predecessor;
  struct Node *successor;
  struct FingerTable *finger_table;
} Node;

/* defines */
#define KEY_BITS 8
#define TRUE 1
#define FALSE 0

#endif
