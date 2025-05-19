#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  if (sockfd < 0) {
    printf("You probably need to run this as root (try sudo).\n");
    return EXIT_FAILURE;
  }

  printf("Raw socket created successfully! (fd = %d)\n", sockfd);

  // Remember to close the socket when done
  close(sockfd);
  return EXIT_SUCCESS;
}
