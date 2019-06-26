#include <arpa/inet.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int port = argc == 2 ? atoi(argv[1]) : 7070;

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(sock_fd >= 0);

  struct sockaddr_in sock = {
    .sin_family = AF_INET,
    .sin_port = htons(port)
  };

  assert(1 == inet_pton(AF_INET, "127.0.0.1", &sock.sin_addr.s_addr));

  int err = connect(sock_fd, (struct sockaddr *) &sock, sizeof sock);
  assert(!err);

  ssize_t N = 0;
  char buf[4096];
  while (0 < (N = read(STDIN_FILENO, buf, sizeof buf)))
    assert(N == write(sock_fd, buf, N));

  close(sock_fd);
}
