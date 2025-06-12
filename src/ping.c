#include "../includes/ft-ping.h"

void ping() {
  printf("PING %s (%s) %d(%d) bytes of data.\n", ctx->target, ctx->target_ip, PAYLOAD_SIZE, PACKET_SIZE);
}
