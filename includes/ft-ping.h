#pragma once

#include <arpa/inet.h>
#include <math.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PACKET_SIZE 64
#define USAGE "Usage: ft_ping [-v verbose] [-h help] hostname\n"

typedef struct s_signals {
  bool should_stop;  // SIGINT
  bool running;      // SIGALARM
} t_signals;

typedef struct s_packet {
  char buffer[PACKET_SIZE];
  struct iphdr *ip_header;
  struct icmphdr *icmp_header;
  size_t size;
} t_packet;

typedef struct s_response {
  struct iovec iov;
  struct msghdr msg;
  char control_buffer[256];
} t_response;

typedef struct s_timing {
  struct timeval start;
  struct timeval end;
  double rtt_ms;
  double min_rtt;
  double max_rtt;
  double sum_rtt;
  double sum_squared_rtt;
} t_timing;

typedef struct s_stats {
  int packets_sent;
  int packets_received;
  int packets_lost;
  double packet_loss_percent;
} t_stats;

typedef struct s_context {
  // Network configuration
  char *target;
  char target_ip[INET_ADDRSTRLEN];
  struct sockaddr_in *target_addr;
  int socket_fd;

  // Packet management
  t_packet packet;
  t_response response;

  // Identification and sequencing
  pid_t process_id;
  int sequence_number;

  // Program options and configuration
  int ping_count;
  int ttl_value;
  bool verbose_mode;
  bool infinite_mode;

  // Runtime state
  t_timing timing;
  t_stats statistics;
  t_signals signals;

  // Additional runtime info
  int received_bytes;
  bool first_packet;
} t_context;

extern t_context *ctx;

void init();
void parse(int argc, char **argv);
void setup_signal_handlers();

// Utils
void fatal_error(char *errorstr);
