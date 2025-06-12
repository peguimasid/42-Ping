#include "../includes/ft-ping.h"

void handle_sigint(int signal) {
  (void)signal;
  ctx->signals.should_stop = true;
  // TODO: Print statistics
}

void handle_sigalrm(int signal) {
  (void)signal;
  ctx->signals.running = true;
}

void trap_signals() {
  signal(SIGINT, handle_sigint);
  signal(SIGALRM, handle_sigalrm);
}
