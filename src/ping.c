#include "../includes/ft-ping.h"

void print_ping_header() {
  printf("PING %s (%s) %d(%d) bytes of data.\n", ctx->target, ctx->target_ip, PAYLOAD_SIZE, PACKET_SIZE);
}

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

void ping_loop() {
  while (!ctx->signals.should_stop) {
    if (ctx->signals.running) {
      printf("pinging\n");
      ctx->signals.running = false;
      alarm(1);
    }
  }
}

void ping() {
  create_socket();
  print_ping_header();
  ping_loop();
}
