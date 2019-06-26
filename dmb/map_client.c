#include "./map.h" 
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  assert(argc > 1 && argc % 2);
  map_t *m = NULL;

  for (int i = 1; argv[i] && argv[i + 1]; i += 2)
    M_set(&m, argv[i], argv[i + 1]);
    
  for (map_t *node = m; node; node = node->next)
    printf("%s -> %s\n", node->key, node->val);
}
