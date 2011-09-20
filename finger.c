#include "finger.h"

Finger* finger_init(Node *node, int start) {
  Finger *finger;

  if ((finger = malloc(sizeof(Finger))) == NULL) {
    BAIL("Failed to allocate memory for Finger");
  }

  finger->node = node;
  finger->start = start;

  return finger;
}

FingerTable* finger_table_init(Node *node) {
  int i;
  Finger *finger;
  FingerTable *finger_table;

  /* allocate finger table memory */
  if ((finger_table = malloc(sizeof(FingerTable))) == NULL) {
    BAIL("Failed to allocate memory for FingerTable");
  }

  finger_table->length = KEY_BITS;

  /* allocate fingers in the table */
  if ((finger_table->fingers = malloc(sizeof(Finger) * finger_table->length)) == NULL) {
    BAIL("Failed to allocate memory for Finger");
  }

  for (i = 0; i < finger_table->length; i++) {
    int init = key_init(node, i);
    finger = finger_init(node, init);
    finger_table->fingers[i] = finger;
  }

  return finger_table;
}
