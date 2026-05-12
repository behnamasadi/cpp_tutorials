// Educational examples for Google Benchmark.
// Mirrors docs/google_benchmark.md.

#include <benchmark/benchmark.h>
#include <string>
#include <vector>

// Example 1: Benchmarking a simple function.
// The for-range loop is executed many times by the framework.
static void StringCreation(benchmark::State& state) {
  for (auto _ : state) {
    std::string s("Hello, World!");
  }
}
BENCHMARK(StringCreation);

// Example 2: Benchmarking with custom arguments.
// state.range(0) returns the value supplied via ->Arg(...).
static void CustomArguments(benchmark::State& state) {
  int n = state.range(0);
  for (auto _ : state) {
    std::vector<int> v(n);
  }
}
BENCHMARK(CustomArguments)->Arg(1024)->Arg(2048)->Arg(4096);

// Example 3: Using fixtures for setup and teardown.
class MyBenchmarkFixture : public benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) override {
    // Code to set up before each benchmark iteration.
  }

  void TearDown(const ::benchmark::State& state) override {
    // Code to clean up after each benchmark iteration.
  }
};

BENCHMARK_F(MyBenchmarkFixture, ExampleBenchmark)(benchmark::State& state) {
  for (auto _ : state) {
    // Benchmark code.
  }
}

// Example 4: Measuring throughput.
// DoNotOptimize prevents the compiler from removing unused work.
// SetItemsProcessed lets the report show items/second.
static int some_operation() { return 42; }

static void BM_Throughput(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(some_operation());
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Throughput);

BENCHMARK_MAIN();
