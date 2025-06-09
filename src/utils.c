#include "../includes/ft-ping.h"

void fatal_error(char *errorstr) {
  printf(errorstr);
  if (ctx) free(ctx);
  exit(EXIT_FAILURE);
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
