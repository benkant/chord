#include "finger.h"

Finger* finger_init(Node *node, int start) {
  Finger *finger;
  finger->node = node;
  finger->start = start;

  return finger;
}

void finger_table_init(FingerTable *table, Node *node) {
  int i;
  Finger *current_finger;
  table->length = KEY_BITS;

  for (i = 0; i < table->length; i++) {
    int init = key_init(node, i);
    current_finger = finger_init(node, init);
    table->fingers[i] = current_finger;
  }
}
