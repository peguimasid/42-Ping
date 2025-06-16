#include "../includes/ft-ping.h"

void create_socket() {
  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  if (sockfd < 0) {
    fatal_error("Socket file descriptor not received");
  }

  int optval = 1;
  int result = setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval));

  if (result < 0) {
    fatal_error("Failed to set socket options");
  }

  ctx->socket_fd = sockfd;
}

void ping() {
  create_socket();
  printf("PING %s (%s) %d(%d) bytes of data.\n", ctx->target, ctx->target_ip, PAYLOAD_SIZE, PACKET_SIZE);
  printf("socket_fd: %d\n", ctx->socket_fd);
}
