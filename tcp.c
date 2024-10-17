#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 80

#define MAX_LINE 4096

#define SA struct sockaddr

void err_n_die(const char *fmt, ...);

int main(int argc, char **argv) {
  int sockfd, n;
  int sendbytes;
  struct sockaddr_in servaddr;
  char sendline[MAX_LINE];
  char recvline[MAX_LINE];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("Sockfd val = %d\n", sockfd);

  // usage check
  if (argc != 2) {
    err_n_die("usage: %s <server address>", argv[0]);
  }

  if (sockfd < 0) {
    err_n_die("Error while creating socked");
  }

  bzero(&servaddr, sizeof(servaddr));     // zero-ing the address
  servaddr.sin_family = AF_INET;          // specify the address
  servaddr.sin_port = htons(SERVER_PORT); // specify the port

  int translatedAddress = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  printf("Translated Address = %d\n", translatedAddress);

  if (translatedAddress < 0) {
    err_n_die("inet_pton error for %s", argv[1]);
  }

  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr))) {
    err_n_die("Connection failed");
  }

  // If connected, prepare the message
  sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
  sendbytes = strlen(sendline);

  // Add to socket
  if (write(sockfd, sendline, sendbytes) != sendbytes) {
    err_n_die("Write Error");
  }

  // Read the server response
  while ((n = read(sockfd, recvline, MAX_LINE - 1)) > 0) {
    printf("%s", recvline);
    memset(recvline, 0, MAX_LINE); // zero-ing the address
  }

  if (n < 0) {
    err_n_die("Read Error");
  }

  exit(0);
}

void err_n_die(const char *fmt, ...) {
  int errno_save;
  va_list ap;

errno_save:
  errno;
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  fprintf(stdout, "\n");
  fflush(stdout);

  if (errno_save != 0) {
    fprintf(stdout, "(errno=%d): %s\n", errno_save, strerror(errno_save));
    fprintf(stdout, "\n");
    fflush(stdout);
  }

  va_end(ap);

  exit(1);
}
