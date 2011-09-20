#include "key.h"

int key_is_between(int check, int from, int to) {
  /* @TODO: refactor */
  if ((from - to) < 0) {
    if (((check - from) > 0) && ((check - to) < 0)) {
      return TRUE;
    }
  } 
  else if ((from - to) > 0) {
    if (((check - to) < 0) || ((check - from) > 0)) {
      return TRUE;
    }
  }
  return FALSE;
}

int key_init(Node *node, int idx) {
  /* @TODO: used? check all functions all called */
  return 1;
}
