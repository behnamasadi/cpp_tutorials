// Demos for docs/date_time.md
// Tip: measure total runtime with `time ./date_time`

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <thread>

// 1. C-style time: std::time, gmtime, asctime
void demoCTime() {
  std::cout << "--- 1. C-style time (ctime) ---\n";
  std::time_t epoch = std::time(nullptr);
  std::cout << "seconds since epoch: " << epoch << '\n';

  std::tm *t = std::gmtime(&epoch);
  std::cout << "UTC date: " << 1900 + t->tm_year << '/' << 1 + t->tm_mon << '/'
            << t->tm_mday << '\n';

  std::cout << "asctime: " << std::asctime(std::gmtime(&epoch));
}

// 2.1 Clock classes: system_clock, steady_clock, high_resolution_clock
void demoClocks() {
  std::cout << "\n--- 2. Clock classes ---\n";
  auto sys = std::chrono::system_clock::now();
  auto stdy = std::chrono::steady_clock::now();
  auto hires = std::chrono::high_resolution_clock::now();

  std::cout << "system_clock ticks:          "
            << sys.time_since_epoch().count() << '\n';
  std::cout << "steady_clock ticks:          "
            << stdy.time_since_epoch().count() << '\n';
  std::cout << "high_resolution_clock ticks: "
            << hires.time_since_epoch().count() << '\n';
  std::cout << "(different epochs/periods, so values are not comparable)\n";
}

// 2.2 Duration: Rep + Period (std::ratio)
void demoDuration() {
  std::cout << "\n--- 3. Duration (Rep, Period) ---\n";
  std::chrono::duration<int, std::ratio<1, 1>> one_second(1);
  std::chrono::duration<int, std::ratio<1, 1000>> one_millisecond(1);
  std::chrono::hours one_hour(1);

  std::cout << "1 s  = " << one_second.count() << " (ratio 1/1)\n";
  std::cout << "1 ms = " << one_millisecond.count() << " (ratio 1/1000)\n";
  std::cout << "1 h  = " << one_hour.count() << " (ratio 3600/1)\n";
}

// 2.3 time_point_cast: convert a time_point to a different duration
void demoTimePointCast() {
  std::cout << "\n--- 4. time_point_cast ---\n";
  auto now = std::chrono::system_clock::now();

  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>
      tp_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
  std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
      tp_s = std::chrono::time_point_cast<std::chrono::seconds>(now);

  std::cout << "as nanoseconds: " << tp_ns.time_since_epoch().count() << '\n';
  std::cout << "as seconds:     " << tp_s.time_since_epoch().count() << '\n';
}

// 2.4 duration_cast + to_time_t/from_time_t
void demoDurationCast() {
  std::cout << "\n--- 5. duration_cast and time_t round-trip ---\n";
  const auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);

  std::cout << "to_time_t (seconds): " << t << '\n';
  std::cout << "duration_cast<seconds>(now): "
            << std::chrono::duration_cast<std::chrono::seconds>(
                   now.time_since_epoch())
                   .count()
            << '\n';
  std::cout << "from_time_t(t).time_since_epoch(): "
            << std::chrono::system_clock::from_time_t(t)
                   .time_since_epoch()
                   .count()
            << '\n';
}

// Bonus: stopwatch idiom with steady_clock + sleep_for
void demoStopwatch() {
  std::cout << "\n--- 6. Stopwatch (steady_clock + sleep_for) ---\n";
  auto start = std::chrono::steady_clock::now();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  auto elapsed = std::chrono::steady_clock::now() - start;
  std::cout << "slept ~10 ms, measured: "
            << std::chrono::duration_cast<std::chrono::microseconds>(elapsed)
                   .count()
            << " us\n";
}

// Bonus: format a time_point with put_time
void demoFormat() {
  std::cout << "\n--- 7. Formatting with put_time ---\n";
  using namespace std::literals;
  auto now = std::chrono::system_clock::now();
  std::time_t t_now = std::chrono::system_clock::to_time_t(now);
  std::time_t t_yesterday = std::chrono::system_clock::to_time_t(now - 24h);

  std::cout << "now:       " << std::put_time(std::localtime(&t_now), "%F %T")
            << '\n';
  std::cout << "yesterday: "
            << std::put_time(std::localtime(&t_yesterday), "%F %T") << '\n';
}

int main() {
  demoCTime();
  demoClocks();
  demoDuration();
  demoTimePointCast();
  demoDurationCast();
  demoStopwatch();
  demoFormat();
  return 0;
}
