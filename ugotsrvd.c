#include "./dmb/map.h"
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 5
#define zero(xs) memset(xs, 0, sizeof xs)

const char *response = 
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/plain; charset=utf-8\r\n"
  "Server: nossl/httpd 1.0\r\n"
  "Date: Wed, 26 Jun 2019 04:26:24 GMT\r\n"
  "Content-Length: 9\r\n"
  "\r\n"
  "1234567\r\n"
;

bool isnumber(char *s) {
  if (!s || !strlen(s))
    return false;

  for (int i = 0; s[i]; i++)
    if (!('0' <= s[i] && s[i] <= '9'))
      return false;

  return true;
}

void normalize(char *s) {
  for (int i = 0; s[i]; i++)
    if ('a' <= s[i] && s[i] <= 'z') // or <ctype.h>
      s[i] ^= 0x20;
    else if (s[i] == '-')
      s[i] = '_';
}

typedef struct request { 
  map_t  *headers;
  size_t  content_length;
  void   *body;
} req_t;

map_t * parse_headers(char *buf) {
  map_t *m;
  for (char *line = strtok(buf, "\r\n"); line; line = strtok(NULL, "\r\n")) {
    int i = 0;
    for (; line[i] && line[i] != ':'; i++)
      ;

    if (!i)
      continue;

    line[i] = '\0';
    char *key = line;
    normalize(key);

    char *val = line + i + 1;
    M_set(&m, key, val);
  }
  return m;
}

void *read_body(int sock, size_t content_length) {
  if (!content_length)
    return NULL;

  void *body = malloc(sizeof(content_length));
  size_t remaining = content_length;

  while (remaining > 0) {
    ssize_t N = read(sock, body, remaining);
    if (N < 0) {
      printf("!!!! %d\n", __LINE__); free(body); return NULL;
    }
    remaining -= N;
  }

  return body;
}

req_t *parse_request(int sock) {
  char buf[4096];
  zero(buf);

  ssize_t N = read(sock, buf, sizeof buf);
  if (N < 0) {
    printf("%d, N < 0\n", __LINE__); return NULL; 
  }

  map_t *headers = parse_headers(buf);

  char *len_str = M_get(headers, "CONTENT_LENGTH");
  size_t content_length;
  if (!isnumber(len_str)) {
    printf("wtf %d\n", __LINE__);
    content_length = 0;
  }
  else {
    content_length = atoi(len_str);
  }

  assert(content_length >= 0);
  void *body = read_body(sock, content_length);

  req_t *req = malloc(sizeof(req_t));
  req->headers        = headers;
  req->content_length = content_length;
  req->body           = body;
  return req;
}

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
    printf("  %d accepted\n", client_fd);
    assert(client_fd >= 0);

    req_t *request = parse_request(client_fd);
    ssize_t N = write(client_fd, response, strlen(response));
    assert(N == strlen(response));

    close(client_fd);
    printf("  %d closed\n", client_fd);
  }
}
