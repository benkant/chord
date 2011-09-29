#include "node.h"

Node* node_init(char *id) {
  Node *node = NULL;
  Ring *ring = ring_get();
  
  if ((node = malloc(sizeof(Node))) == NULL) {
    BAIL("Failed to allocate memory for Node");
  }
  
  node->id = id;
  node->key = chord_hash(id);
  node->finger_table = finger_table_init(node);
  node->state = NODE_STATE_RUNNING;
  node->num_documents = 0;
  
  ring->nodes[ring->size] = node;
  ring->size++;
  
  return node;
}

Node* node_find_successor(Node *node, int key) {
  return node_find_successor_impl(node, node, key, 0);
}

Node* node_find_successor_impl(Node *orig_node, Node *node, int key, int depth) {
  Node *closest_preceding_node = NULL;

  depth++;
  
  if (depth > KEY_BITS * 2) {
    return node_find_successor(orig_node->successor, key);
  }
  
  if (key_in_range(key, node->key, node->successor->key, TRUE)
      || node == node->successor) {
    return node->successor;
  }
  else {
    closest_preceding_node = node_closest_preceding_node(node, key);
    if (closest_preceding_node == node) {
      return node_find_successor_impl(orig_node, node->successor, key, depth);
    }
    return node_find_successor_impl(orig_node, closest_preceding_node, key, depth);
  }
}

Node* node_closest_preceding_node(Node *node, int key) {
  int i;
  Finger *finger = NULL;
  
  for (i = KEY_BITS - 1; i >= 0; i--) {
    finger = node->finger_table->fingers[i];
    
    if (key_in_range(finger->start, node->key, key, FALSE)) {
      return finger->node;
    }
  }
  
  return node;
}

void node_create(Node *node) {
  Ring *ring = ring_get();
  
  ring->first_node = node;
  ring->last_node = node;
  node->predecessor = NULL;
  node->successor = node;
}

void node_join(Node *existing_node, Node *new_node) {
  new_node->predecessor = NULL;
  new_node->successor = node_find_successor(existing_node, new_node->key);
}

void node_stabilise(Node *node) {
  Node *successor = node->successor;
  Node *x = node->successor->predecessor;
  int i = 0;
  
  /* per E.3 maintain successor list */
  while (successor != NULL && i < SUCCESSOR_LIST_SIZE) {
    node->successors[i] = successor;
    successor = successor->successor;
    i++;
  }
  
  if (x != NULL) {
    if (node == node->successor
        || key_in_range(x->key, node->key, node->successor->key, FALSE)) {
      node->successor = x;
    }
  }
  node_notify(node->successor, node);
}

void node_notify(Node *notify_node, Node *check_node) {
  if ((notify_node->predecessor == NULL 
       || key_in_range(check_node->key, notify_node->predecessor->key, notify_node->key, FALSE))) {
    
    /* check_node thinks it might be notify_node's predecessor */
    notify_node->predecessor = check_node;
  }
}

void node_fix_fingers(Node *node) {
  int i;
  Finger *finger = NULL;
  Node *nodes[KEY_BITS];
  
  /* reset */
  for (i = 0; i < KEY_BITS; i++) {
    finger = node->finger_table->fingers[i];
    finger->node = node->successor;
  }
  
  for (i = 0; i < KEY_BITS; i++) {
    finger = node->finger_table->fingers[i];
    nodes[i] = node_find_successor(node, finger->start);
    /*
    finger = node->finger_table->fingers[i];
    finger->node = node_find_successor(node, finger->start);
    */
  }
  
  for (i = 0; i < KEY_BITS; i++) {
    finger = node->finger_table->fingers[i];
    finger->node = nodes[i];
  }
  
  /*for (i = 0; i < KEY_BITS; i++) {*/
  /*
  for (i = KEY_BITS - 1; i >= 0; i--) {
    finger = node->finger_table->fingers[i];
    finger->node = node_find_successor(node, finger->start);
  }
  */
}

void node_check_predecessor(Node *node) {
  if (node->predecessor->state == NODE_STATE_DEAD) {
    node->predecessor = NULL;
  }
}

/**
 * Node wants to add a document to the chord ring.
 * Search for the node responsible for this key and
 * store it at the target node.
 */
void node_document_add(Node *node, Document *doc) {
  Node *target;
  
  target = node_find_successor(node, doc->key);
  node_document_store(target, doc);
}

/**
 * Store a document at this node
 */
void node_document_store(Node *node, Document *doc) {
  if ((node->documents = realloc(node->documents, (sizeof(struct Document*) * node->num_documents + 1))) == NULL) {
    BAIL("Failed to allocate memory for node documents");
  }
  
  node->documents[node->num_documents] = doc;
  node->num_documents++;
  
  printf("Document \"%s\" with key %d added to node %s:%d\n", doc->filename, doc->key, node->id, node->key);
}

void node_document_query(Node *ctx_node, char *filename) {
  int key;
  Node *doc_node;
  Document *doc = NULL;
  
  key = chord_hash(filename);
  
  doc_node = node_find_successor(ctx_node, key);
  
  doc = node_document_exists(doc_node, filename);
  
  printf("\n");
  
  if (doc != NULL) {
    printf("Document found. Displaying data...\n");
    node_document_print(doc_node, doc);
  }
  else {
    printf("Document not found.\n");
  }
}

void node_document_print(Node *node, Document *doc) {
  printf("Filename: %s\n", doc->filename);
  printf("Data:\n %s\n", doc->data);
}

Document* node_document_exists(Node *node, char *filename) {
  int i;
  
  for (i = 0; i < node->num_documents; i++) {
    if (strcmp(filename, node->documents[i]->filename) == 0) {
      return node->documents[i];
    }
  }
  
  return NULL;
}

void node_print(Node *node) {
  int predecessor, successor;
  
  predecessor = node->predecessor != NULL ? node->predecessor->key : 0;
  successor = node->successor != NULL ? node->successor->key : 0;

  printf("%-4d %-11s %5d %5d %7d\n", node->key, node->id, predecessor, successor, node->num_documents);
}

void node_print_documents(Node *node) {
  int i;
  Document *doc;
  
  if (node->num_documents == 0) {
    printf("\nNo documents at this node.\n");
  }
  else {
    printf("\n");
    printf("%-3s %-4s %-16s\n", "i", "Key", "Filename");
    printf("--- ---- ----------------\n");
    for (i = 0; i < node->num_documents; i++) {
      doc = node->documents[i];
      printf("%-3d %-4d %s\n", i, doc->key, doc->filename);
    }
    printf("--- ---- ----------------\n");
  }
}

void node_print_finger_table(Node *node) {
  int i;
  Finger *finger = NULL;
  
  printf("\n");
  printf("%-3s %-6s %-16s\n", "i", "Start", "Succ (ID:Key)");
  printf("--- ------ ----------------\n");
  
  for (i = 0; i < KEY_BITS; i++) {
    finger = node->finger_table->fingers[i];
    printf("%-3d %-6d %10s : %-4d\n", i, finger->start, finger->node->id, finger->node->key);
  }
  printf("--- ------ ----------------\n");
}
