#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define PACKET_SIZE 64

unsigned short checksum(void *b, int len) {
  unsigned short *buf = b;
  unsigned int sum = 0;
  unsigned short result;

  for (sum = 0; len > 1; len -= 2) sum += *buf++;
  if (len == 1) sum += *(unsigned char *)buf;
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;
  return result;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./ft_ping <hostname>\n");
    return EXIT_FAILURE;
  }

  // Perform DNS resolution
  struct addrinfo hints;
  struct addrinfo *res;
  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;

  int status = getaddrinfo(argv[1], NULL, &hints, &res);

  if (status != 0) {
    perror("getaddrinfo");
    return EXIT_FAILURE;
  }

  struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
  char ipstr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(addr->sin_addr), ipstr, INET_ADDRSTRLEN);
  printf("PING %s (%s) 56(84) bytes of data.\n", argv[1], ipstr);

  char packet[PACKET_SIZE];
  struct icmphdr *icmp = (struct icmphdr *)packet;
  unsigned short seq_no = 1;

  // Create socket
  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  if (sockfd < 0) {
    perror("socket");
    freeaddrinfo(res);
    return EXIT_FAILURE;
  }

  // Fill ICMP header
  bzero(packet, PACKET_SIZE);
  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.id = getpid() & 0xFFFF;
  icmp->un.echo.sequence = seq_no;
  icmp->checksum = 0;
  icmp->checksum = checksum(packet, PACKET_SIZE);

  // Send packet
  ssize_t sent = sendto(sockfd, packet, PACKET_SIZE, 0, res->ai_addr, res->ai_addrlen);

  if (sent < 0) {
    perror("sendto");
    close(sockfd);
    freeaddrinfo(res);
    return EXIT_FAILURE;
  }

  // Try to receive a reply (with 1 second timeout)
  fd_set readfds;
  struct timeval timeout = {1, 0};
  FD_ZERO(&readfds);
  FD_SET(sockfd, &readfds);
  int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);

  if (ready == 0) {
    printf("Request timeout for icmp_seq 1\n");
    close(sockfd);
    freeaddrinfo(res);
    return EXIT_FAILURE;
  }

  if (ready < 0) {
    perror("select");
    close(sockfd);
    freeaddrinfo(res);
    return EXIT_FAILURE;
  }

  char recvbuf[1024];
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  ssize_t recvd = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from, &fromlen);

  if (recvd <= 0) {
    perror("recvfrom");
    return EXIT_FAILURE;
  }

  printf("Received a response from %s\n", argv[1]);

  close(sockfd);
  return EXIT_SUCCESS;
}
