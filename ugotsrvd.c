#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int port = argc == 2 ? atoi(argv[1]) : 7070;
  printf("listening on %s:%d\n", "0.0.0.0", port);
}
