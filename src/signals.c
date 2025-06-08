#include "../includes/ft-ping.h"

void handle_signal(int signal) {
  (void)signal;
}

void set_signal_handlers() {
  signal(SIGINT, handle_signal);
  signal(SIGALRM, handle_signal);
}
