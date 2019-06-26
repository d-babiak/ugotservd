#ifndef DMB_MAP
#define DMB_MAP

#include <stdbool.h>

typedef struct m_node {
  struct m_node *next;
  char *key;
  char *val;
} map_t;

void  M_set(map_t **m, char *k, char *v);

char *M_get(map_t  *m, char *k);

#endif
