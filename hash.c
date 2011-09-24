#include "hash.h"

int chord_hash(char *string) {
  int i, hash = 0;
  
  for (i = 0; i < strlen(string); i++) {
    hash = hash * 31 + string[i];
  }
  
  hash = abs(hash);
  
  return hash % ring_key_max();
}

