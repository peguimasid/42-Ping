#include "../includes/ft-ping.h"

void fatal_error(char *errorstr) {
  printf(errorstr);
  if (ctx) free(ctx);
  exit(EXIT_FAILURE);
}

void set_address_by_target(char *target) {
  struct addrinfo hints;
  struct addrinfo *result;

  bzero(&hints, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;

  int status = getaddrinfo(target, NULL, &hints, &result);

  if (status != 0) {
    printf("ft_ping: %s: Name or service not known\n", target);
    free(ctx);
    exit(EXIT_FAILURE);
  }

  ctx->target_addr = (struct sockaddr_in *)result->ai_addr;
  inet_ntop(AF_INET, &(ctx->target_addr->sin_addr), ctx->target_ip, INET_ADDRSTRLEN);

  freeaddrinfo(result);
}

void handle_signal(int signal) {
  if (signal == SIGINT) {
    ctx->signals.should_stop = true;
    // TODO: Print statistics
  }

  if (signal == SIGALRM) {
    ctx->signals.running = true;
  }
}

void set_signal_handlers() {
  signal(SIGINT, handle_signal);
  signal(SIGALRM, handle_signal);
}

t_context *init_context(void) {
  ctx = malloc(sizeof(t_context));
  bzero(ctx, sizeof(t_context));

  ctx->packet.ip_header = (struct iphdr *)ctx->packet.buffer;
  ctx->packet.icmp_header = (struct icmphdr *)(ctx->packet.ip_header + 1);
  ctx->packet.size = PACKET_SIZE;

  ctx->process_id = getpid();
  ctx->sequence_number = 1;

  ctx->timing.min_rtt = 0.0;
  ctx->timing.max_rtt = 0.0;
  ctx->timing.sum_squared_rtt = 0.0;
  ctx->timing.sum_rtt = 0.0;
  ctx->timing.rtt_ms = 0.0;

  ctx->ttl_value = 64;
  ctx->ping_count = -1;
  ctx->infinite_mode = true;
  ctx->verbose_mode = false;

  ctx->signals.should_stop = false;
  ctx->signals.running = true;

  ctx->socket_fd = -1;
  ctx->first_packet = true;
  ctx->received_bytes = 0;
  ctx->target = NULL;

  ctx->statistics.packets_sent = 0;
  ctx->statistics.packets_received = 0;
  ctx->statistics.packets_lost = 0;
  ctx->statistics.packet_loss_percent = 0.0;

  return ctx;
}
