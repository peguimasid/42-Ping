#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  struct addrinfo hints;
  struct addrinfo *result;
  char ipstr[INET_ADDRSTRLEN];

  if (argc != 2) {
    printf("Usage: %s <hostname> or ipv4\n", argv[0]);
    return EXIT_FAILURE;
  }

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET;  // IPv4

  if (getaddrinfo(argv[1], NULL, &hints, &result) != 0) {
    perror("getaddrinfo");
    return EXIT_FAILURE;
  }
  struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
  inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));

  printf("Resolved %s to %s\n", argv[1], ipstr);
  freeaddrinfo(result);  // Free the linked list

  return EXIT_SUCCESS;
}
