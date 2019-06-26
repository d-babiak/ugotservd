#include "./map.h"
#include <string.h>
#include <stdlib.h>

#define streq(x,y) (x && y && !strcmp(x, y))

static map_t * M_get_node(map_t *m, char *k) {
  for (map_t *node = m; node; node = node->next)
    if (streq(node->key, k))
      return node;
  return NULL;
}

void  M_set(map_t **m, char *k, char *v) {
  map_t *node = M_get_node(*m, k);
  if (node)
    node->val = strdup(v);
  else {
    node = malloc(sizeof(map_t));
    node->next = *m;
    node->key  = strdup(k);
    node->val  = strdup(v);
    *m = node;       
  }
}

char *M_get(map_t *m, char *k) {
  map_t *node = M_get_node(m, k);
  return node ? node->val : NULL; 
}
