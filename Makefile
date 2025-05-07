NAME = ft_ping

# Run in Linux environment using Docker
linux: linux-clean
	@docker compose -f docker/compose.yml run debian bash

# Clean Docker environment
linux-clean:
	@docker compose -f docker/compose.yml down --remove-orphans
