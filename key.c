#include "key.h"

int key_in_range(int check, int bound1, int bound2) {
  int min = MIN(bound1, bound2);
  int max = MAX(bound1, bound2);
  int result;

  /* per III.C of the paper range is (a, b] */
  result = (check > min && check <= max);

  return result;
}

int key_init(Node *node, int idx) {
  /* @TODO: what is this supposed to do? */
  return idx;
}
