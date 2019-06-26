#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 5
#define zero(xs) memset(xs, 0, sizeof xs)

int main(int argc, char *argv[]) {
  int port = argc == 2 ? atoi(argv[1]) : 7070;
  printf("listening on %s:%d\n", "0.0.0.0", port);

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(listen_fd >= 0);

  struct sockaddr_in sock = {
    .sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr = {.s_addr = INADDR_ANY}
  };

  int err = bind(listen_fd, (struct sockaddr *) &sock, sizeof sock);
  assert(!err);

  err = listen(listen_fd, BACKLOG);
  assert(!err);


  for (;;) {
    struct sockaddr_in client_sock = {};
    socklen_t sock_len;
    int client_fd = accept(listen_fd, (struct sockaddr *) &client_sock, &sock_len);
    assert(client_fd >= 0);

    char buf[4096];
    ssize_t N = 0;

    while (0 < (N = read(client_fd, buf, sizeof buf)))
      assert(N == write(STDOUT_FILENO, buf, N));

    close(client_fd);
  }
}
