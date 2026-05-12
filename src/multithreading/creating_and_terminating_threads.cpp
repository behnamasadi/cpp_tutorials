// Demonstrates §2.1–2.6 of docs/multithreading.md: the different ways to
// hand a callable to std::jthread.

#include <iostream>
#include <string>
#include <syncstream>
#include <thread>

// osyncstream keeps each line atomic when several threads share std::cout.
void greet(int frame_id) {
  std::osyncstream(std::cout)
      << "[free fn]   processing frame " << frame_id << '\n';
}

class SensorReader {
public:
  void read(int id, char bus) const {
    std::osyncstream(std::cout)
        << "[member fn] reader " << id << " on bus '" << bus << "'\n";
  }
};

struct FramePrinter {
  void operator()(int id) const {
    std::osyncstream(std::cout) << "[functor]   frame " << id << '\n';
  }
};

void relabel(std::string &label) { label = "tracker.front_left"; }

int main() {
  // Function pointer with a forwarded argument.
  std::jthread t1(greet, 42);

  // Member function on a heap-free instance. The instance is decay-copied
  // by default; pass &reader to share, std::ref(reader) to bind a reference.
  SensorReader reader;
  std::jthread t2(&SensorReader::read, &reader, 1, 'A');

  // Function object.
  std::jthread t3(FramePrinter{}, 7);

  // Lambda — the common style today.
  int n = 10;
  std::jthread t4(
      [n] { std::osyncstream(std::cout) << "[lambda]    n = " << n << '\n'; });

  // Move-only ownership: t5 transfers the thread to t6, t5 is left empty.
  std::jthread t5([] {
    std::osyncstream(std::cout) << "[move]      running on the moved thread\n";
  });
  std::jthread t6 = std::move(t5);

  // Pass by reference via std::ref. Without ref(), `label` would bind to
  // an internal copy and the original would stay "imu_raw".
  std::string label = "imu_raw";
  {
    std::jthread t7(relabel, std::ref(label));
  } // joined here
  std::cout << "[ref]       label after thread: " << label << '\n';

  // All other jthreads join when main returns.
}
