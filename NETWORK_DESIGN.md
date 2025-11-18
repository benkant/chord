# Network Connectivity Design for Chord DHT using NNG

## Executive Summary

This document outlines the design for adding real network connectivity to the Chord DHT implementation using the nng (nanomsg-next-generation) library.

**Current State:** Pure in-memory simulation with direct pointer access between nodes  
**Target State:** Distributed system with network RPC communication  
**Estimated Effort:** 7-11 days of development

---

## Design principles (as enforced by review)

Think of this codebase the same way you would think of a debugger or game engine:

- **Layer correctness over cleverness.**  
  Network code lives in `src/net`, core Chord logic lives in `src/core`, UI lives in `src/app`. Violations will be rejected.

- **Data structures first, API second.**  
  The ring, finger tables, and node state representations must be explicit and easy to inspect. APIs are built around these structures, not the other way around.

- **Observability is not optional.**  
  Logging and tracing hooks must be designed into the system. You should be able to trace a lookup from CLI command, through the node state machine, across the network, and back.

- **Tests are the spec.**  
  Behavior is defined by tests. When behavior is ambiguous, write/adjust a test first, then adjust the code.

- **No dead code, no speculative abstractions.**  
  If an abstraction is not clearly used, it should not exist. Keep the design focused on the Chord protocol and its immediate needs.

During code review, changes will be evaluated primarily against:

1. Does this respect the layer boundaries?
2. Does this make the system easier to debug and test?
3. Is the behavior covered by unit and/or integration tests?

---

## 1. Repository layout (target)

The repository is structured into layers and subsystems. The goal is:
- clear separation of concerns
- testable units
- explicit boundaries between transport, protocol, and application

Target layout:

- `src/`
  - `core/`
    - `chord_types.h` – core type definitions, IDs, configuration
    - `hash.c`, `hash.h` – consistent hashing, key space operations
    - `key.c`, `key.h` – key representation, parsing, formatting
    - `ring.c`, `ring.h` – Chord ring operations (join, stabilize, lookup)
    - `finger.c`, `finger.h` – finger table maintenance and selection
    - `node.c`, `node.h` – node life‑cycle, state machine, timers
  - `net/`
    - `net_transport.c`, `net_transport.h` – raw network I/O (sockets or NNG)
    - `net_protocol.c`, `net_protocol.h` – wire format, message encode/decode
    - `net_peer.c`, `net_peer.h` – high‑level peer abstraction wrapping transport
  - `app/`
    - `app_driver.c` – CLI / TUI / test harness entry point
    - `app_commands.c`, `app_commands.h` – user commands to interact with the ring
  - `util/`
    - `util.c`, `util.h` – shared utilities (logging, time, random IDs, etc.)
- `include/`
  - Public headers, if we export an API (e.g. `chord.h`).
- `tests/`
  - `unit/` – small unit tests for `src/core` and `src/util`
  - `integration/` – multi‑node network tests using the test harness
- `build/`
  - Out‑of‑tree build artifacts (optional, but recommended).
- `scripts/`
  - Helper scripts for running test clusters, benchmarks, etc.
- `vendor/`
  - `nng/` – nng library as git submodule

Existing source files (e.g. `ring.c`, `node.c`) are moved into these directories without changing their responsibilities initially. Follow‑up refactors will split them further along these boundaries.

**File migration map:**
- `chord_types.h` → `src/core/chord_types.h`
- `hash.c`, `hash.h` → `src/core/hash.c`, `src/core/hash.h`
- `key.c`, `key.h` → `src/core/key.c`, `src/core/key.h`
- `ring.c`, `ring.h` → `src/core/ring.c`, `src/core/ring.h`
- `finger.c`, `finger.h` → `src/core/finger.c`, `src/core/finger.h`
- `node.c`, `node.h` → `src/core/node.c`, `src/core/node.h`
- `util.c`, `util.h` → `src/util/util.c`, `src/util/util.h`
- `app_driver.c` → `src/app/app_driver.c`
- `test_hash.c` → `tests/unit/test_hash.c`

---

## 2. Current Architecture Analysis

### Communication Model
- **Type:** In-memory simulation (no actual network calls)
- **Method:** Direct pointer dereferencing between nodes
- **Storage:** All nodes in global `Ring` structure with `Node *nodes[500]` array
- **Limitation:** All nodes must exist in same process memory space

### Key Operations Requiring Network Conversion

#### Core Chord Protocol (8 operations)
1. `node_find_successor()` - Recursive key lookup
2. `node_closest_preceding_node()` - Finger table query
3. `node_stabilise()` - Periodic stabilization
4. `node_notify()` - Predecessor notification
5. `node_fix_fingers()` - Finger table maintenance
6. `node_join()` - Bootstrap into network
7. `node_document_add()` - Store document
8. `node_document_query()` - Retrieve document

---

## 3. Build and toolchain

The project targets a modern C toolchain:

- C standard: **C23 preferred**, minimum **C20**.
- We expect a conforming libc implementation (e.g. glibc, musl, modern Windows CRT).
- The code must compile cleanly with:
  - `clang` and `gcc` on Linux/macOS
  - `clang-cl` or `cl` on Windows (where feasible)

Recommended baseline compiler flags:

```sh
# Development build
cc -std=c2x -Wall -Wextra -Wpedantic -Werror \
   -Wshadow -Wconversion -Wdouble-promotion -Wformat=2 \
   -fno-common -fstrict-aliasing -g -O0 \
   -fsanitize=address,undefined \
   -o build/chord src/app/app_driver.c ...

# Release build
cc -std=c2x -Wall -Wextra -Wpedantic \
   -O3 -DNDEBUG \
   -o build/chord src/app/app_driver.c ...
```

The existing `makefile` will be replaced with:

- A minimal `Makefile` that:
  - builds `build/chord` from `src/**`
  - builds unit tests under `build/tests/*`
  - exposes targets:
    - `all` – default build
    - `debug` – debug build with sanitizers
    - `release` – optimized build
    - `test` – build and run the full test suite
    - `clean` – remove build artifacts
- Optionally, a small CMake configuration if IDE support is desired; the `Makefile` remains the authoritative reference.

---

## 4. Architectural layers and boundaries

The Chord implementation is organized into **directional layers**. Dependencies flow **upwards** only:

### 1. Foundation / platform layer
- Files: `src/util/*`, `src/net/net_transport.*`
- Responsibilities:
  - OS and libc abstractions: time, random IDs, logging, error handling.
  - Network transport primitives: sockets or NNG endpoints, connection management.
- May depend on system headers and external libraries (e.g. `nng`), but **not** on Chord concepts (keys, rings, nodes).

### 2. Chord core layer
- Files: `src/core/chord_types.h`, `src/core/hash.*`, `src/core/key.*`, `src/core/ring.*`, `src/core/finger.*`, `src/core/node.*`
- Responsibilities:
  - Key space definition (identifier size, hash function).
  - Ring topology and invariants.
  - Node state machine and stabilization algorithm.
  - Lookup protocol (find successor/predecessor) abstracted over a generic messaging API.
- Depends only on the **foundation** layer for utilities (time, randomness) and on `net_protocol` interfaces, not on any UI or CLI.

### 3. Network protocol layer
- Files: `src/net/net_protocol.*`, `src/net/net_peer.*`
- Responsibilities:
  - Encode/decode of protocol messages (join, stabilize, notify, lookup, data transfer).
  - Framing, versioning, and error codes.
  - Mapping between Chord core requests and low‑level transport messages.
- Depends on **foundation** (transport) and **Chord core** (types and commands), but is decoupled through stable interfaces:
  - `net_protocol.h` defines the wire types.
  - `net_peer.h` exposes async operations (connect, send_request, receive_response, close).

### 4. Application / UI layer
- Files: `src/app/app_driver.*`, `src/app/app_commands.*`
- Responsibilities:
  - Start and stop a node.
  - Configure the node (ports, join addresses, logging).
  - Provide a CLI interface and simple scripts for integration tests.
- Depends on **Chord core** and **network protocol** layers, but not vice versa.

### Dependency rules

- `src/core/*` must not include `src/app/*`.
- `src/core/*` may depend on narrow interfaces in `src/net/net_protocol.h` or `src/net/net_peer.h`, but not on raw sockets.
- `src/app/*` is allowed to know about everything below, but must not contain algorithmic logic that belongs in `src/core/*`.

---

## 5. C language and libc guidelines

The codebase targets **modern C**:

- Compile with `-std=c2x` (or `-std=c20` where C23 is not yet available).
- Assume a modern libc (POSIX‑like APIs on Unix, Winsock + CRT on Windows).

Allowed features:

- Designated initializers and compound literals for structs.
- Fixed‑width integer types (`uint32_t`, `uint64_t`, etc.) from `<stdint.h>`.
- `_Static_assert` for invariants that must hold at compile time.
- Inline functions in headers for small, performance‑critical helpers.
- Flexible array members where appropriate, coupled with explicit length tracking.

Memory and safety:

- No naked `malloc`/`free` calls scattered through the code. All allocations go through narrow utility functions in `src/util/memory.*`.
- All public APIs define clear ownership rules for allocations, and these rules are documented in header comments.
- Use `size_t` for sizes and counts, avoid mixing signed and unsigned types; use casts consciously and locally.

Error handling:

- No silent error ignoring. Every call that can fail must return an error code or log explicitly.
- Use a simple `enum chord_err` (or reuse `nng_err` style) for domain‑specific error codes.
- Functions that can fail return an error code; out‑parameters are only written on success.

Standard library usage:

- Prefer standard functions (`memcpy`, `memmove`, `snprintf`, `strtoul`, `clock_gettime` or equivalent) over custom re‑implementations, unless there is a clear portability or performance reason.
- Wrap non‑portable system calls behind small adapters in `src/util` or `src/net`.

---

## 6. Coding conventions (for humans and tools)

When writing C for this project, follow these rules:

### Headers
- Every `.c` file has exactly one corresponding `.h` file for its public interface, except pure implementation files used only by a single translation unit.
- Header files declare types and functions; they do not define storage except for `static` or `inline` helpers.

### Interfaces
- Functions that are part of a module's public API are prefixed with the module name (`chord_ring_`, `chord_node_`, `net_peer_`, etc.).
- Internal helpers are declared `static` and live near their only call sites.

### Functions
- Keep functions small and single‑purpose. If it does more than one conceptual task, split it.
- All functions that can fail return an error code; do not mix return codes and `errno`.
- Avoid global mutable state. When state is necessary (e.g. a node's state), keep it in explicit context structs passed around.

### Style
- Use K&R brace style consistently.
- No trailing whitespace, use spaces (not tabs), 4-space indentation.
- Use descriptive names: `node`, `finger_table`, `successor`, not `n`, `ft`, `s`.

### Comments
- Document the *contract* of each public function in the header: inputs, outputs, ownership, error conditions.
- Avoid redundant comments. The code should be readable without comments; comments explain *why*, not *what*.

### Testing
- For new functionality, write tests before or alongside implementation:
  - unit tests for pure logic (hashing, ring operations)
  - integration tests for network behavior and multi‑node scenarios

---

## 7. Testing and TDD strategy

The test suite is split into **unit tests** and **integration tests**. We aim for a TDD workflow:

1. **Write tests that describe the desired behavior.**
2. Implement the minimal code to make them pass.
3. Refactor while keeping tests green.

### Unit tests

- Located under `tests/unit/`.
- Focus: deterministic, in‑process tests for:
  - `hash.*` – consistent hashing, ring size constraints.
  - `key.*` – key parsing/formatting, comparison.
  - `ring.*` – successor/predecessor logic, interval arithmetic.
  - `finger.*` – finger table index calculation and updates.
  - `node.*` – state transitions; use fake timers and fake network peers.
- Should not use real sockets. Instead:
  - Introduce a `net_peer_iface` that can be implemented by a fake.
  - Unit tests inject fake peers to validate protocol logic.

Example unit test layout:

- `tests/unit/test_hash.c`
- `tests/unit/test_ring.c`
- `tests/unit/test_node_state.c`

Each test file follows a simple test framework (e.g. `acutest`‑style or a small in‑house equivalent) for consistency.

### Integration tests

- Located under `tests/integration/`.
- Focus: real network behavior and multi‑node scenarios.
- Use a small test harness inspired by NNG's `nuts.h`:
  - Helpers to create temporary addresses (`chord_test_scratch_addr()`).
  - Helpers to start and stop nodes (`chord_test_spawn_node()` / `chord_test_kill_node()`).
  - Macros to connect nodes and verify connectivity (`CHORD_TEST_MARRY()` equivalent).
  - Utilities to wait for stabilization and check invariants (`CHORD_TEST_WAIT_STABLE()`).

The test harness should:

- Allow running multiple integration tests in a single process without resource leaks (similar spirit to `TEST_INIT` / `TEST_FINI` in `nuts.h`).
- Provide `CHORD_TEST_PASS` / `CHORD_TEST_FAIL` macros mirroring `NUTS_PASS` / `NUTS_FAIL`, wrapping the project's error codes.
- Provide short, readable macros for common patterns:
  - `CHORD_TEST_SEND_LOOKUP(node, key, expected_successor)`
  - `CHORD_TEST_RING_INVARIANTS(nodes[], count)`

Integration test examples:

- `tests/integration/test_two_node_join.c`
- `tests/integration/test_stabilization_under_churn.c`
- `tests/integration/test_key_distribution.c`

### Test runner and automation

- A single `test` target in the `Makefile`:
  - builds all tests to `build/tests/*`
  - runs unit tests first, then integration tests
- On failure, tests must print enough logging (using the project's logging facilities) to diagnose the issue without reruns.

---

## 8. NNG Library Overview

### Key Features
- **Lightweight:** Broker-less messaging library
- **Protocols:** REQ/REP (request-reply), PUB/SUB, PUSH/PULL, etc.
- **Transports:** TCP, IPC, inproc, TLS, WebSocket
- **Async I/O:** Built-in asynchronous operations
- **License:** MIT (compatible with this project)

### Why NNG for Chord?
1. **REQ/REP pattern** perfect for RPC-style Chord operations
2. **Async I/O** enables concurrent request handling
3. **Multiple transports** (TCP for distributed, IPC for local testing)
4. **TLS support** for secure production deployments
5. **Simple API** minimizes integration complexity

---

## 9. Proposed Architecture

### 9.1 Network Layer Components

```
┌─────────────────────────────────────────────────────────┐
│                    Application Layer                     │
│              (app_driver.c - user interface)             │
└─────────────────────────────────────────────────────────┘
                            │
┌─────────────────────────────────────────────────────────┐
│                   Chord Protocol Layer                   │
│    (node.c, ring.c - modified to use network calls)     │
└─────────────────────────────────────────────────────────┘
                            │
┌─────────────────────────────────────────────────────────┐
│                  Network Abstraction Layer               │
│         (network.c - RPC client functions)               │
│  ┌──────────────────────────────────────────────────┐   │
│  │  RPC Server (rpc_server.c - request handlers)    │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
                            │
┌─────────────────────────────────────────────────────────┐
│                      NNG Library                         │
│              (REQ/REP sockets over TCP/IPC)              │
└─────────────────────────────────────────────────────────┘
```

### 9.2 Data Structures

#### NodeAddress (replaces Node* for remote references)
```c
typedef struct NodeAddress {
    char id[NODE_ID_LENGTH + 1];
    int key;
    char url[256];  /* e.g., "tcp://192.168.1.1:5555" */
} NodeAddress;
```

#### Modified Node Structure
```c
typedef struct Node {
    char *id;
    int key;
    NodeAddress *predecessor;      /* Was: Node* */
    NodeAddress *successor;        /* Was: Node* */
    FingerTable *finger_table;
    int state;
    Document **documents;
    int num_documents;
    NodeAddress *successors[SUCCESSOR_LIST_SIZE];
    
    /* NEW: Network components */
    char bind_url[256];
    nng_socket rep_socket;         /* Server socket for incoming RPCs */
    nng_thread *rpc_thread;        /* RPC server thread */
} Node;
```

### 9.3 RPC Message Protocol

#### Message Format (JSON for simplicity)
```json
{
    "type": "FIND_SUCCESSOR",
    "request_id": "uuid",
    "params": {
        "key": 42
    }
}

{
    "type": "FIND_SUCCESSOR_RESPONSE",
    "request_id": "uuid",
    "result": {
        "id": "node1",
        "key": 42,
        "url": "tcp://192.168.1.1:5555"
    }
}
```

#### RPC Types
1. `FIND_SUCCESSOR` - Find successor for key
2. `GET_PREDECESSOR` - Get predecessor node
3. `GET_SUCCESSOR` - Get successor node
4. `NOTIFY` - Notify of potential predecessor
5. `CLOSEST_PRECEDING` - Get closest preceding node
6. `STORE_DOCUMENT` - Store document
7. `QUERY_DOCUMENT` - Query document
8. `PING` - Health check

---

## 10. Implementation Plan

### Phase 1: Foundation (2-3 days)
**Files:** `network.h`, `network.c`, `serialize.h`, `serialize.c`

1. Create NodeAddress structure
2. Implement JSON serialization/deserialization
3. Create network initialization functions
4. Implement basic RPC client (REQ socket)
5. Add timeout and error handling

### Phase 2: RPC Server (2-3 days)
**Files:** `rpc_server.h`, `rpc_server.c`

1. Create RPC server thread
2. Implement REP socket listener
3. Add request dispatcher
4. Implement handlers for 8 RPC types
5. Add thread-safe access to local node state

### Phase 3: Core Integration (2-3 days)
**Files:** Modify `node.c`, `ring.c`, `chord_types.h`

1. Update Node structure with network fields
2. Replace Node* with NodeAddress* in function signatures
3. Replace direct pointer access with network_* calls
4. Update node_init() to start RPC server
5. Add node cleanup to stop RPC server

### Phase 4: Testing & Refinement (3-5 days)
**Files:** `test_network.c`, update `makefile`

1. Create unit tests for network layer
2. Test single-node operation
3. Test two-node operation
4. Test multi-node ring formation
5. Test document storage/retrieval
6. Test node failure scenarios
7. Performance testing and optimization

---

## 11. Key Design Decisions

### 11.1 Synchronous vs Asynchronous RPC
**Decision:** Start with synchronous, add async optimization later
- **Rationale:** Simpler to implement and debug
- **Future:** Use nng_aio for async operations to improve throughput

### 11.2 Message Serialization Format
**Decision:** JSON for initial implementation
- **Pros:** Human-readable, easy to debug, standard libraries available
- **Cons:** Larger message size, slower parsing
- **Future:** Consider Protocol Buffers or MessagePack for production

### 11.3 Transport Protocol
**Decision:** TCP for distributed nodes, IPC for local testing
- **TCP:** `tcp://0.0.0.0:5555` for production
- **IPC:** `ipc:///tmp/chord_node_1` for local multi-node testing
- **Future:** Add TLS transport for secure deployments

### 11.4 Thread Safety
**Decision:** Use nng's built-in thread safety + minimal locking
- Each node has dedicated RPC server thread
- Use mutex for document storage access
- Node state reads are atomic (int fields)

### 11.5 Error Handling Strategy
**Decision:** Comprehensive timeout and retry logic
- All RPC calls have 5-second timeout
- Failed RPCs return NULL/error code
- Caller decides retry strategy (e.g., try successor list)

---

## 12. Code Examples

### 12.1 Network Initialization
```c
int network_init(Node *node, const char *bind_url) {
    int rv;
    
    /* Create REP socket for incoming requests */
    if ((rv = nng_rep0_open(&node->rep_socket)) != 0) {
        return rv;
    }
    
    /* Bind to address */
    if ((rv = nng_listen(node->rep_socket, bind_url, NULL, 0)) != 0) {
        nng_close(node->rep_socket);
        return rv;
    }
    
    /* Store bind URL */
    strncpy(node->bind_url, bind_url, sizeof(node->bind_url) - 1);
    
    /* Start RPC server thread */
    if ((rv = nng_thread_create(&node->rpc_thread, rpc_server_loop, node)) != 0) {
        nng_close(node->rep_socket);
        return rv;
    }
    
    return 0;
}
```

### 12.2 RPC Client Call
```c
NodeAddress* network_find_successor(NodeAddress *remote, int key) {
    nng_socket sock;
    nng_msg *msg;
    char request[512];
    char *response;
    NodeAddress *result = NULL;
    int rv;
    
    /* Create REQ socket */
    if ((rv = nng_req0_open(&sock)) != 0) {
        return NULL;
    }
    
    /* Set timeout */
    nng_socket_set_ms(sock, NNG_OPT_RECVTIMEO, 5000);
    
    /* Connect to remote node */
    if ((rv = nng_dial(sock, remote->url, NULL, 0)) != 0) {
        nng_close(sock);
        return NULL;
    }
    
    /* Build request */
    snprintf(request, sizeof(request), 
             "{\"type\":\"FIND_SUCCESSOR\",\"params\":{\"key\":%d}}", key);
    
    /* Send request */
    if ((rv = nng_msg_alloc(&msg, 0)) != 0 ||
        (rv = nng_msg_append(msg, request, strlen(request))) != 0 ||
        (rv = nng_sendmsg(sock, msg, 0)) != 0) {
        nng_close(sock);
        return NULL;
    }
    
    /* Receive response */
    if ((rv = nng_recvmsg(sock, &msg, 0)) == 0) {
        response = (char*)nng_msg_body(msg);
        result = deserialize_node_address(response);
        nng_msg_free(msg);
    }
    
    nng_close(sock);
    return result;
}
```

### 12.3 RPC Server Handler
```c
void handle_find_successor_rpc(Node *local, nng_msg *request) {
    char *request_body = (char*)nng_msg_body(request);
    int key;
    Node *successor;
    char response[512];
    
    /* Parse request */
    key = parse_find_successor_request(request_body);
    
    /* Execute local Chord operation */
    successor = node_find_successor(local, key);
    
    /* Build response */
    snprintf(response, sizeof(response),
             "{\"type\":\"FIND_SUCCESSOR_RESPONSE\","
             "\"result\":{\"id\":\"%s\",\"key\":%d,\"url\":\"%s\"}}",
             successor->id, successor->key, successor->bind_url);
    
    /* Send response */
    nng_msg_clear(request);
    nng_msg_append(request, response, strlen(response));
    nng_sendmsg(local->rep_socket, request, 0);
}
```

---

## 13. Migration Strategy

### Backward Compatibility
Keep simulation mode for testing:
```c
#ifdef NETWORK_MODE
    result = network_find_successor(remote_addr, key);
#else
    result = node_find_successor(remote_node, key);
#endif
```

### Gradual Rollout
1. **Week 1:** Implement network layer, test with 2 nodes
2. **Week 2:** Full integration, test with 5-10 nodes
3. **Week 3:** Stress testing, optimization, documentation

---

## 14. Additional Testing Strategy

### Unit Tests
- Serialization/deserialization correctness
- RPC timeout handling
- Error condition handling

### Integration Tests
- Two-node ring formation
- Multi-node ring formation (5, 10, 20 nodes)
- Document storage and retrieval
- Node join/leave operations
- Stabilization convergence

### Performance Tests
- Lookup latency (average, p95, p99)
- Throughput (requests/second)
- Scalability (10, 50, 100 nodes)

---

## 15. Future Enhancements

1. **TLS Transport:** Secure communication for production
2. **Async I/O:** Use nng_aio for better concurrency
3. **Compression:** Reduce message size for large documents
4. **Monitoring:** Add metrics and observability
5. **NAT Traversal:** Support nodes behind firewalls
6. **IPv6 Support:** Modern network compatibility

---

## 16. Build System Changes

### Updated Makefile
```makefile
CC=clang
CFLAGS=-std=c2x -Wall -Wextra -Wpedantic -Werror \
       -Wshadow -Wconversion -Wdouble-promotion -Wformat=2 \
       -fno-common -fstrict-aliasing
CFLAGS_DEBUG=-g -O0 -fsanitize=address,undefined
CFLAGS_RELEASE=-O3 -DNDEBUG
LDFLAGS=-lm -Lvendor/nng/build -lnng -lpthread
INCLUDES=-Iinclude -Ivendor/nng/include

SRC_CORE=src/core/hash.c src/core/key.c src/core/ring.c \
         src/core/finger.c src/core/node.c
SRC_NET=src/net/net_transport.c src/net/net_protocol.c src/net/net_peer.c
SRC_UTIL=src/util/util.c
SRC_APP=src/app/app_driver.c src/app/app_commands.c

OBJS=$(SRC_CORE:.c=.o) $(SRC_NET:.c=.o) $(SRC_UTIL:.c=.o) $(SRC_APP:.c=.o)

all: build/chord

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: build/chord

release: CFLAGS += $(CFLAGS_RELEASE)
release: build/chord

build/chord: $(OBJS)
	@mkdir -p build
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: build/tests/unit/test_hash
	./build/tests/unit/test_hash

build/tests/unit/test_hash: tests/unit/test_hash.c src/core/hash.o src/core/ring.o
	@mkdir -p build/tests/unit
	$(CC) $(CFLAGS) $(INCLUDES) $^ $(LDFLAGS) -o $@

clean:
	rm -rf build/ $(OBJS)

.PHONY: all debug release test clean
```

### Dependencies
- **nng:** Install via package manager or build from source
  - Ubuntu/Debian: `apt-get install libnng-dev`
  - macOS: `brew install nng`
  - From source: `git clone https://github.com/nanomsg/nng && cd nng && mkdir build && cd build && cmake .. && make && sudo make install`

---

## 17. Conclusion

This design provides a clear path to transform the Chord simulation into a real distributed system. The nng library offers the perfect balance of simplicity and power for implementing the required RPC communication patterns.

**Next Steps:**
1. Install nng library
2. Create network.c with basic RPC client
3. Create rpc_server.c with request handlers
4. Modify node.c to use network calls
5. Test with 2-node setup
6. Expand to multi-node testing

**Estimated Timeline:** 7-11 days for full implementation and testing
