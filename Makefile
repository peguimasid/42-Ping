NAME = ft_ping

# Compiler options
CC = @gcc
CFLAGS = -Wall -Wextra -Werror

# Source files
SRCS = $(shell find src -name '*.c')

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(NAME)

# Build the executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "✓ $(NAME)"

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files
clean:
	@rm -f $(OBJS)
	@echo "✕ Object files removed"

# Clean all generated files
fclean: clean
	@rm -rf $(NAME)
	@echo "✕ $(NAME) removed"

# Rebuild everything
re:	fclean all

# Run tests
test: all
	@./$(NAME)
	@rm -f $(OBJS)
	@rm -rf $(NAME)

# Run in Linux environment using Docker
linux: linux-clean
	@docker compose -f docker/compose.yml run debian bash

# Clean Docker environment
linux-clean:
	@docker compose -f docker/compose.yml down --remove-orphans

.PHONY:	all clean fclean re
