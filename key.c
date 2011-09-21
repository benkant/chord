#include "key.h"

int key_in_range(int check, int bound1, int bound2) {
  int min = MIN(bound1, bound2);
  int max = MAX(bound1, bound2);

  /* per III.C of the paper range is (a, b] */
  return (check > min && check <= max);
}

int key_init(Node *node, int idx) {
  /* @TODO: what is this supposed to do? */
  return idx;
}
