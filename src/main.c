#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
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

  // Validate the ICMP response
  struct iphdr *ip_header = (struct iphdr *)recvbuf;
  int ip_header_len = ip_header->ihl * 4;
  struct icmphdr *icmp_reply = (struct icmphdr *)(recvbuf + ip_header_len);

  // Check if the ICMP reply is an echo reply and matches our request
  bool is_echo_reply = icmp_reply->type == ICMP_ECHOREPLY;
  bool is_matching_id = icmp_reply->un.echo.id == (getpid() & 0xFFFF);
  bool is_matching_seq = icmp_reply->un.echo.sequence == seq_no;

  if (is_echo_reply && is_matching_id && is_matching_seq) {
    char from_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(from.sin_addr), from_ip, INET_ADDRSTRLEN);
    printf("64 bytes from %s: icmp_seq=%d ttl=%d\n", from_ip, seq_no, ip_header->ttl);
  } else {
    printf("Received an invalid response\n");
    close(sockfd);
    freeaddrinfo(res);
    return EXIT_FAILURE;
  }

  close(sockfd);
  freeaddrinfo(res);
  return EXIT_SUCCESS;
}
