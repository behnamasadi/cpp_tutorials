/*
How to get the execution time of any program on Linux Shell:
$time ./<your_application>
 */

#include <iostream>
#include <time.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>

template <typename Clock, typename Duration>
std::ostream &
operator<<(std::ostream &stream,
           const std::chrono::time_point<Clock, Duration> &time_point) {
  const time_t time = Clock::to_time_t(time_point);
  struct tm tm;
  localtime_r(&time, &tm);
  return stream << std::put_time(&tm, "%c"); // Print standard date&time
}

void timeLapseFirstExample() {
  auto start = std::chrono::high_resolution_clock::now();
  std::this_thread::sleep_for(std::chrono::microseconds(1000));
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  auto microseconds =
      std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  std::cout << "time lapse in microseconds:" << microseconds << std::endl;
}

void cTime() {
  time_t epoch = std::time(NULL);

  std::cout << "seconds since the epoch began: " << epoch << std::endl;

  tm *t = gmtime(&epoch);

  std::cout << "current date and time: " << 1900 + t->tm_year << "/"
            << 1 + t->tm_mon << "/" << t->tm_mday << std::endl;

  std::cout << "current date and time: " << asctime(gmtime(&epoch))
            << std::endl;
}

int main() {
  using namespace std::literals;
  const std::chrono::time_point<std::chrono::system_clock> now =
      std::chrono::system_clock::now();

  const std::time_t t_c = std::chrono::system_clock::to_time_t(now - 24h);
  std::cout << "24 hours ago, the time was "
            << std::put_time(std::localtime(&t_c), "%F %T.\n") << std::flush;

  const std::chrono::time_point<std::chrono::steady_clock> start =
      std::chrono::steady_clock::now();

  std::cout << "Different clocks are not comparable: \n"
               "  System time: "
            << now.time_since_epoch()
            << "\n"
               "  Steady time: "
            << start.time_since_epoch() << '\n';

  // std::chrono::time_point now1 = std::chrono::system_clock::now();
  // std::cout << now0.time_since_epoch() << std::endl;

  // std::chrono::time_point<std::chrono::system_clock> now1 =
  //     std::chrono::system_clock::now();

  // std::cout << now1 << std::endl;

  // // std::chrono::time_point<std::chrono::steady_clock> now2 =
  // //     std::chrono::system_clock::now();

  // std::time_t t_c =
  //     std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  // std::cout << t_c << std::endl;

  // std::chrono::nanoseconds is : typedef duration<signed int, std::nano>
  // nanoseconds;

  // timePoint = std::chrono::system_clock::now();

  // std::chrono::system_clock::to_time_t(timePoint);

  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>
      timePoint = std::chrono::time_point_cast<std::chrono::nanoseconds>(
          std::chrono::system_clock::now());

  std::cout << timePoint << std::endl;
  // std::cout << std::chrono::system_clock::to_time_t(timePoint) << std::endl;

  std::cout << timePoint.time_since_epoch() << std::endl;

  {
    const auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    std::cout << time << "\n";
    std::cout << std::chrono::duration_cast<std::chrono::seconds>(
                     now.time_since_epoch())
                     .count()
              << "\n";
    std::cout << std::chrono::system_clock::from_time_t(time)
                     .time_since_epoch()
                     .count()
              << "\n";
    std::cout << std::chrono::system_clock::time_point(
                     std::chrono::seconds(time))
                     .time_since_epoch()
                     .count()
              << "\n";
  }

  return 0;
}
