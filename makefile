all:
	gcc -Wall -Wextra -Iinclude src/main.c src/banker.c src/ipc.c src/worker.c src/resource_limits.c src/scheduler.c src/logger.c -o campuscompute -pthread -lrt

run: all
	./campuscompute

clean:
	rm -f campuscompute src/*.o

.PHONY: all run clean