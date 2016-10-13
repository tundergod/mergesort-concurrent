CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
OBJS = list.o threadpool.o main.o

.PHONY: all clean test

GIT_HOOKS := .git/hooks/pre-commit

all: $(GIT_HOOKS) sort

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

auto:
	for i in `seq 10000 10000 1000000`; do \
		for j in 1 2 4 8 16 32 64; do \
			for i in {1..10000}; do echo $$RANDOM; \
					./sort $$j $$i; \
				done \
			done \
		done
		
check:
	for i in 100; do \
		for j in 4; do \
			for i in {1..100}; do echo $$RANDOM; \
			./sort $$j $$i; \
				done \
			done \
		done
clean:
	rm -f $(OBJS) sort
	@rm -rf $(deps)

-include $(deps)
