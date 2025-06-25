#include "../includes/ft-ping.h"

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
}

void handle_arg(char *arg) {
  if (strcmp(arg, "-h") == 0) {
    printf(USAGE);
    free(ctx);
    exit(EXIT_SUCCESS);
  }

  if (strcmp(arg, "-v") == 0) {
    ctx->verbose_mode = true;
    return;
  }

  if (strcmp(arg, "-") == 0) {
    fatal_error(USAGE);
    return;
  }

  ctx->target = arg;

  set_address_by_target(ctx->target);
}

void parse(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    handle_arg(argv[i]);
  }

  if (!ctx->target_addr) {
    fatal_error("ft_ping: usage error: Destination address required\n");
  }
}
