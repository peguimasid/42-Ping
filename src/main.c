#include <arpa/inet.h>        // Provides definitions for internet operations
#include <errno.h>            // For accessing the error number
#include <netdb.h>            // For network database operations like DNS resolution
#include <netinet/in.h>       // Internet address family structures
#include <netinet/ip_icmp.h>  // ICMP header definitions
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  // Socket interface functions
#include <sys/time.h>    // Time-related functions
#include <unistd.h>      // POSIX API for various system calls

#define PACKET_SIZE 64  // Define the size of our ICMP packet (in bytes)

/**
 * Calculate the Internet Checksum of the supplied data
 * The Internet Checksum is used to detect corruption in transmitted packets
 *
 * @param b - Pointer to the data to calculate the checksum for
 * @param len - Length of the data in bytes
 * @return The 16-bit checksum value
 */
unsigned short checksum(void *b, int len) {
  unsigned short *buf = b;
  unsigned int sum = 0;
  unsigned short result;

  // Sum up 16-bit words
  for (sum = 0; len > 1; len -= 2)
    sum += *buf++;

  // If there's a byte left over, add it
  if (len == 1)
    sum += *(unsigned char *)buf;

  // Add back any carries
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);

  // Take one's complement of the sum
  result = ~sum;
  return result;
}

int main(int argc, char **argv) {
  // Check if we have the correct number of command line arguments
  if (argc != 2) {
    printf("Usage: ./ft_ping <hostname>\n");
    return EXIT_FAILURE;
  }

  // =============================================================================
  // DNS RESOLUTION SECTION
  // =============================================================================
  // Set up the hints structure for getaddrinfo()
  struct addrinfo hints, *res;
  bzero(&hints, sizeof(hints));  // Zero out the structure
  hints.ai_family = AF_INET;     // IPv4 only
  hints.ai_socktype = SOCK_RAW;  // Raw socket type

  // Perform the DNS resolution to convert hostname to IP address
  int status = getaddrinfo(argv[1], NULL, &hints, &res);
  if (status != 0) {
    // If getaddrinfo fails, the hostname couldn't be resolved
    printf("ft_ping: %s: Name or service not known\n", argv[1]);
    return EXIT_FAILURE;
  }

  // Extract the IP address and convert it to a string for display
  struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
  char ipstr[INET_ADDRSTRLEN];  // Buffer to hold the IP address string
  inet_ntop(AF_INET, &(addr->sin_addr), ipstr, INET_ADDRSTRLEN);

  // Display initial ping information, similar to standard ping
  printf("PING %s (%s) 56(84) bytes of data.\n", argv[1], ipstr);

  // =============================================================================
  // ICMP PACKET PREPARATION AND PING LOOP
  // =============================================================================
  char packet[PACKET_SIZE];                         // Buffer for our ICMP packet
  struct icmphdr *icmp = (struct icmphdr *)packet;  // Cast the buffer to ICMP header structure

  // Create a raw socket for sending ICMP packets
  // IPPROTO_ICMP specifies we're using the ICMP protocol
  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  if (sockfd < 0) {
    // If socket creation fails, report the error
    perror("socket");
    freeaddrinfo(res);  // Clean up the address info
    return EXIT_FAILURE;
  }

  for (int i = 1; i < 4; i++) {
    struct timeval start_time, end_time;

    gettimeofday(&start_time, NULL);

    // Fill in the ICMP header fields
    bzero(packet, PACKET_SIZE);                      // Zero out the packet buffer
    icmp->type = ICMP_ECHO;                          // 8 - Echo Request
    icmp->code = 0;                                  // Code 0 for Echo Request
    icmp->un.echo.id = getpid() & 0xFFFF;            // Use process ID as identifier (truncated to 16 bits)
    icmp->un.echo.sequence = i;                      // Sequence number
    icmp->checksum = 0;                              // Zero out checksum before calculating
    icmp->checksum = checksum(packet, PACKET_SIZE);  // Calculate and set the checksum

    // Send the packet to the destination
    ssize_t sent = sendto(sockfd, packet, PACKET_SIZE, 0, res->ai_addr, res->ai_addrlen);

    if (sent < 0) {
      // If sending fails, report the error
      perror("sendto");
      close(sockfd);      // Clean up the socket
      freeaddrinfo(res);  // Clean up the address info
      return EXIT_FAILURE;
    }

    // Wait for response with timeout
    fd_set readfds;
    struct timeval timeout = {1, 0};  // 1 second timeout
    FD_ZERO(&readfds);                // Clear the set
    FD_SET(sockfd, &readfds);         // Add our socket to the set

    // Wait for data to be available on the socket (with timeout)
    int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);

    if (ready == 0) {
      printf("Request timeout for icmp_seq %d\n", i);
      continue;  // Try next ping instead of exiting
    }

    if (ready < 0) {
      // If select fails, report the error
      perror("select");
      close(sockfd);
      freeaddrinfo(res);
      return EXIT_FAILURE;
    }

    // Receive and validate response
    char recvbuf[1024];       // Buffer for the received packet (IP header + ICMP)
    struct sockaddr_in from;  // Address of the sender
    socklen_t fromlen = sizeof(from);

    // Receive the packet
    ssize_t recvd = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from, &fromlen);

    if (recvd <= 0) {
      // If receiving fails, report the error
      perror("recvfrom");
      continue;  // Try next ping instead of exiting
    }

    gettimeofday(&end_time, NULL);
    double rtt = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_usec - start_time.tv_usec) / 1000.0;

    // Process the received packet
    struct iphdr *ip_header = (struct iphdr *)recvbuf;
    int ip_header_len = ip_header->ihl * 4;  // IP header length in bytes (ihl gives it in 4-byte units)

    // Get a pointer to the ICMP part of the received packet
    struct icmphdr *icmp_reply = (struct icmphdr *)(recvbuf + ip_header_len);

    // Validate the ICMP reply
    bool is_echo_reply = icmp_reply->type == ICMP_ECHOREPLY;
    bool is_matching_id = icmp_reply->un.echo.id == (getpid() & 0xFFFF);
    bool is_matching_seq = icmp_reply->un.echo.sequence == i;

    if (is_echo_reply && is_matching_id && is_matching_seq) {
      char from_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(from.sin_addr), from_ip, INET_ADDRSTRLEN);
      printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", from_ip, i, ip_header->ttl, rtt);
    } else {
      printf("Received an invalid response\n");
    }

    if (i < 4) {
      sleep(1);
    }
  }

  close(sockfd);
  freeaddrinfo(res);
  return EXIT_SUCCESS;
}
