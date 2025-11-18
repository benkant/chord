# Chord DHT Implementation - Build System
# Modern C (C20/C23) with strict warnings and sanitizers

CC=gcc
CFLAGS=-std=c2x -Wall -Wextra -Wpedantic -Werror \
       -Wshadow -Wconversion -Wdouble-promotion -Wformat=2 \
       -fno-common -fstrict-aliasing
CFLAGS_DEBUG=-g -O0 -fsanitize=address,undefined
CFLAGS_RELEASE=-O3 -DNDEBUG
LDFLAGS=-lm
INCLUDES=-Isrc/core -Isrc/util -Isrc/app

# Source files (new structure)
SRC_CORE=src/core/hash.c src/core/key.c src/core/ring.c src/core/finger.c src/core/node.c
SRC_UTIL=src/util/util.c
SRC_APP=src/app/app_driver.c
OBJS_CORE=$(SRC_CORE:.c=.o)
OBJS_UTIL=$(SRC_UTIL:.c=.o)
OBJS_APP=$(SRC_APP:.c=.o)
OBJS=$(OBJS_CORE) $(OBJS_UTIL) $(OBJS_APP)

# Test files
TEST_HASH=build/tests/unit/test_hash
TEST_KEY=build/tests/unit/test_key
TEST_RING=build/tests/unit/test_ring

.PHONY: all debug release test clean help

all: chord

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: chord

release: CFLAGS += $(CFLAGS_RELEASE)
release: chord

# Main executable
chord: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

# Object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Unit tests
test: test-hash test-key test-ring
	@echo ""
	@echo "=== All unit tests passed ==="

test-hash: $(TEST_HASH)
	@echo "Running hash unit tests..."
	@./$(TEST_HASH)

test-key: $(TEST_KEY)
	@echo "Running key unit tests..."
	@./$(TEST_KEY)

test-ring: $(TEST_RING)
	@echo "Running ring unit tests..."
	@./$(TEST_RING)

$(TEST_HASH): tests/unit/test_hash.c $(OBJS_CORE) $(OBJS_UTIL)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $^ $(LDFLAGS) -o $@

$(TEST_KEY): tests/unit/test_key.c $(OBJS_CORE) $(OBJS_UTIL)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $^ $(LDFLAGS) -o $@

$(TEST_RING): tests/unit/test_ring.c $(OBJS_CORE) $(OBJS_UTIL)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $(INCLUDES) $^ $(LDFLAGS) -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) chord chord_debug
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
