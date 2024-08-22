## Google Benchmark
Google Benchmark is a micro-benchmarking library developed by Google that helps you measure the performance of individual code snippets or small parts of a program. It is particularly useful for performance-critical code where you need to optimize specific functions or algorithms. It automatically determines the number of iterations required to get an accurate measurement. The library provides detailed output of the benchmarking results, including mean, median, and standard deviation.

### Example 1: Benchmarking a Simple Function

```cpp
#include <benchmark/benchmark.h>

// Function to benchmark
void StringCreation(benchmark::State& state) {
  for (auto _ : state) {
    std::string s("Hello, World!");
  }
}

// Register the function as a benchmark
BENCHMARK(StringCreation);

// Main function to run the benchmarks
BENCHMARK_MAIN();
```

**Explanation**:
- The `StringCreation` function creates a string in each iteration.
- The `for (auto _ : state)` loop runs the benchmark repeatedly. The `_` variable is a convention for unused variables, and the loop runs as many times as necessary to get a precise measurement.
- `BENCHMARK(StringCreation);` registers the function as a benchmark.
- `BENCHMARK_MAIN();` is the entry point for the benchmark program, and it runs all registered benchmarks.

### Example 2: Benchmarking with Custom Arguments

You can also pass custom arguments to benchmarks to evaluate performance under different conditions.

```cpp
#include <benchmark/benchmark.h>

void CustomArguments(benchmark::State& state) {
  int n = state.range(0);
  for (auto _ : state) {
    std::vector<int> v(n);
  }
}

// Register the benchmark with a range of values
BENCHMARK(CustomArguments)->Arg(1024)->Arg(2048)->Arg(4096);

// Main function to run the benchmarks
BENCHMARK_MAIN();
```

**Explanation**:
- The `CustomArguments` function benchmarks the creation of a vector with a size specified by the custom argument `n`.
- `state.range(0)` retrieves the first custom argument.
- `BENCHMARK(CustomArguments)->Arg(1024)->Arg(2048)->Arg(4096);` registers the benchmark and specifies that it should run with vector sizes of 1024, 2048, and 4096 elements.

### Example 3: Using Fixtures for Setup and Teardown

Google Benchmark also supports the use of fixtures, which allow you to set up and tear down test environments before and after the benchmark runs.

```cpp
#include <benchmark/benchmark.h>

class MyBenchmarkFixture : public benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State& state) override {
    // Code to set up before each benchmark iteration
  }

  void TearDown(const ::benchmark::State& state) override {
    // Code to clean up after each benchmark iteration
  }
};

BENCHMARK_F(MyBenchmarkFixture, ExampleBenchmark)(benchmark::State& state) {
  for (auto _ : state) {
    // Benchmark code
  }
}

// Main function to run the benchmarks
BENCHMARK_MAIN();
```

**Explanation**:
- `MyBenchmarkFixture` is a fixture class derived from `benchmark::Fixture`.
- `SetUp` and `TearDown` methods are overridden to provide setup and teardown logic for each benchmark iteration.
- `BENCHMARK_F(MyBenchmarkFixture, ExampleBenchmark)` registers the benchmark within the fixture context.

### Example 4: Measuring Throughput

Sometimes, you might want to measure throughput, such as the number of operations per second.

```cpp
#include <benchmark/benchmark.h>

void BM_Throughput(benchmark::State& state) {
  for (auto _ : state) {
    // Code to benchmark
    benchmark::DoNotOptimize(some_operation());
  }
  state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_Throughput);

BENCHMARK_MAIN();

```
[code](../src/benchmark_demo.cpp)

