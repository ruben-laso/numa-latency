# NUMA Latency Benchmark

This project is a simple program that measures memory latency in NUMA (Non-Uniform Memory Access) systems.
It also serves as a small demo for [libnuma](https://man7.org/linux/man-pages/man3/numa.3.html)

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Installation

To install and run the NUMA Latency Benchmark, follow these steps:

1. Clone the repository:

```shell
git clone https://github.com/your-username/numa-latency.git
```

2. Build the benchmark program:

```shell
cd numa-latency
make
```

## Usage

To run the NUMA Latency Benchmark, execute the following command:

```shell
make run
# or
./numa-latency [input-size]
```

By default, the benchmark uses an input size 4 times the size of the L3 cache. You can specify a custom input size by passing it as an argument to the program.

## Dependencies

- `libnuma`