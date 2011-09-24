#include "key.h"

int key_in_range(int check, int bound1, int bound2, int half) {
  if (bound1 > bound2) {
    if (half) {
      if (check <= bound2 || check > bound1) {
        return TRUE;
      }
    }
    else {
      if (check < bound2 || check > bound1) {
        return TRUE;
      }
    }
  }
  else {
    if (half) {
      if (check > bound1 && check <= bound2) {
        return TRUE;
      }
    }
    else {
      if (check > bound1 && check < bound2) {
        return TRUE;
      }
    }
  }
  
  return FALSE;
  
  /* half enclosed range */
  if (half) {
    /* per III.C of the paper range is (a, b] in find_successor() */
    return (check > bound1 && check <= bound2);
  }
  else {
    return (check > bound1 && check < bound2);
  }
}

int key_init(Node *node, int idx) {
  /* @TODO: what is this supposed to do? */
  return idx;
}
