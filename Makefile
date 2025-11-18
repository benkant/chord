# Chord DHT Implementation - Build System
# Modern C (C20/C23) with strict warnings and sanitizers

CC=gcc
# TODO: Re-enable -Werror after fixing existing warnings
CFLAGS=-std=c2x -Wall -Wextra -Wpedantic \
       -Wshadow -Wconversion -Wdouble-promotion -Wformat=2 \
       -fno-common -fstrict-aliasing
CFLAGS_DEBUG=-g -O0 -fsanitize=address,undefined
CFLAGS_RELEASE=-O3 -DNDEBUG
LDFLAGS=-lm
INCLUDES=-I.

# Source files (current flat structure, will migrate to src/ later)
SRC_CORE=hash.c key.c ring.c finger.c node.c util.c
OBJS=$(SRC_CORE:.c=.o)

# Test files
TEST_HASH=tests/unit/test_hash

.PHONY: all debug release test clean

all: chord

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: chord

release: CFLAGS += $(CFLAGS_RELEASE)
release: chord

# Main executable (current structure)
chord: $(OBJS) app_driver.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

# Object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Unit tests
test: test-hash
	@echo ""
	@echo "=== All unit tests passed ==="

test-hash: $(TEST_HASH)
	@echo "Running hash unit tests..."
	@./$(TEST_HASH)

$(TEST_HASH): tests/unit/test_hash.c hash.o ring.o node.o finger.o key.o util.o
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $^ $(LDFLAGS) -o $@

# Clean build artifacts
clean:
	rm -f *.o chord chord_debug
	rm -rf tests/unit/test_hash tests/integration/*_test
	rm -rf build/

# Help target
help:
	@echo "Chord DHT Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build chord executable (default)"
	@echo "  debug    - Build with debug symbols and sanitizers"
	@echo "  release  - Build optimized release version"
	@echo "  test     - Build and run all unit tests"
	@echo "  clean    - Remove all build artifacts"
	@echo "  help     - Show this help message"
