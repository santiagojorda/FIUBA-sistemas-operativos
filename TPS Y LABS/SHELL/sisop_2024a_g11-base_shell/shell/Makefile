CFLAGS := -ggdb3 -O2 -Wall -Wextra -std=gnu11
CFLAGS += -Wmissing-prototypes

# To force build a test shell run:
#     make -B -e SHELL_TEST=true
ifdef SHELL_TEST
	CFLAGS += -D SHELL_NO_COLORS -D SHELL_NO_INTERACTIVE
endif

EXEC := sh
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)

all: $(EXEC)

$(EXEC): $(OBJS)

valgrind: $(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXEC)

test:
	./tests/run

test-%:
	./tests/run $*

format: .clang-files .clang-format
	xargs -r clang-format -i <$<

clean:
	rm -rf $(EXEC) *.o core vgcore.*

.PHONY: all clean format valgrind test
