# libmalloc

A drop-in replacement for the standard C memory allocator, built in C99. Implements `malloc`, `free`, `realloc`, and `calloc` as a shared library.

## How it works

Two allocation strategies depending on request size:

- **Bucket allocator** - small/medium allocations are served from fixed-size buckets grouped by size class. Reduces fragmentation and speeds up allocation for common sizes.
- **mmap allocator** - large allocations are handled directly with `mmap` / `munmap`, bypassing the bucket system.

## Architecture

```
src/
|-- malloc.c             # malloc / free / realloc / calloc
|-- bucket/              # Bucket-based allocator
`-- big_nmap/            # Large allocation via mmap
```

## Build

```bash
make
```

Produces `libmalloc.so`.

## Usage

Inject into any program without recompilation:

```bash
LD_PRELOAD=./libmalloc.so ./your_program
```

## Tests

```bash
make check
```

Tests cover allocation correctness, memory reuse, heap corruption detection, and memory footprint under load.

---

EPITA - Systems programming (ING1)
