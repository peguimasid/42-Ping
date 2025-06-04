#include "../includes/ft-ping.h"

void fatal_error(char *errorstr) {
  printf("%s\n", errorstr);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  if (getuid() != 0) {
    fatal_error("ft_ping: Operation not permitted (you must be root)\n");
  }

  if (argc != 2) {
    fatal_error("Usage: ft_ping [-v verbose] [-h help] hostname");
  }
}
