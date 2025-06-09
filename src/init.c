#include "../includes/ft-ping.h"

void init() {
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
}
