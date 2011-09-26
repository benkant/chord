#include "finger.h"

Finger* finger_init(Node *node, int start) {
  Finger *finger = NULL;
  
  if ((finger = malloc(sizeof(Finger))) == NULL) {
    BAIL("Failed to allocate memory for Finger");
  }
  
  finger->node = node;
  finger->start = start;
  
  return finger;
}

FingerTable* finger_table_init(Node *node) {
  int i;
  Finger *finger = NULL;
  FingerTable *finger_table = NULL;
  int start;
  
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
    start = node->key + pow(2, i);
    if (start > ring_key_max()) {
      start -= ring_key_max();
    }
    finger = finger_init(node, start);
    finger_table->fingers[i] = finger;
  }
  
  return finger_table;
}
