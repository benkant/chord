#include "hash.h"

int chord_hash(char *string) {
  size_t len = strlen(string);
  int hash = 0;
  
  for (size_t i = 0; i < len; i++) {
    hash = hash * 31 + string[i];
  }
  
  hash = abs(hash);
  
  return hash % ring_key_max();
}

