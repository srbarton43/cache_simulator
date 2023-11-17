# Cache Simulator

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Description

This project simulates four different computer hardware caching methods in software, using the LRU strategy to handle evictions.

## Four Cache Types:

- direct-mapped
- 2-way set-associative
- 4-way set-associative
- fully associative 

## Usage:

```bash
./cache_simulation [-v] -t cache_type trace_file
```

Where -v toggles verbosity, 
cache_type ranges from [0,4],
and trace_file is a line-separated text file of memory addresses (32-bit)

Sample trace files are located in the traces directory

## Building

```bash
make
```

On Unix, simply call `make` in the root directory to compile the executable

## Testing

```bash
make test
```

Tests are located in the `tests/testing.sh` script and are matched against known cache behavior on the sample trace file.

Further efforts have been done with memory tools such as valgrind and gdb to maks sure the code is memory-safe.

## Cleaning the Directory

```bash
make clean
```
