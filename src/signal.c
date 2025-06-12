#include "../includes/ft-ping.h"

void handle_signal(int signal) {
  if (signal == SIGINT) {
    ctx->signals.should_stop = true;
    // TODO: Print statistics
  }

  if (signal == SIGALRM) {
    ctx->signals.running = true;
  }
}

void trap_signals() {
  signal(SIGINT, handle_signal);
  signal(SIGALRM, handle_signal);
}
