# Use the specified base image
FROM debian:bookworm-slim

# Install necessary packages
RUN apt update && \
    apt upgrade -y && \
    apt install -y clang gcc git iputils-ping make man tcpdump valgrind vim && \
    apt clean

WORKDIR /ft-ping
