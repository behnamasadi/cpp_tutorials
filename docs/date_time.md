# 1. Date and Time in C
1. `std::time()`: It returns the current time (seconds since 00:00:00 GMT, Jan 1, 1970) as an object of type `time_t`. `time_t` is defined as `int64` under the hood.

2. `gmtime  (time_t *timer )`: converts given time since epoch into calendar time, expressed in Coordinated Universal Time (UTC) in the `struct tm` format. 
The result is stored in static storage and a pointer to that static storage is returned.


3. `char* asctime (struct tm * timeptr)`: It convert the contents of the `tm` structure into a C-string in the following format: `www Mmm dd hh:mm:ss yyyy`

```cpp
#include <ctime>
#include <time.h>

int main() 
{
  time_t epoch = std::time(NULL);
  std::cout << "seconds since the epoch began: " << epoch << std::endl;
  tm *t = gmtime(&epoch);
  std::cout << "current date and time: " << 1900 + t->tm_year << "/"
            << 1 + t->tm_mon << "/" << t->tm_mday << std::endl;

  std::cout << "current date and time: " << asctime(gmtime(&epoch))
            << std::endl;
}
```

# 2. Date and Time in C++

`template <class Clock, class Duration>  class time_point;`: This class expresses a point in time relative to a clock's epoch.

## 2.1 Clock class:
1. `system_clock`: is a system-wide realtime clock.
2. `steady_clock`:  is specifically designed to calculate time intervals
3. `high_resolution_clock`:  is the clock with the shortest tick period. It may be a synonym for `system_clock` or `steady_clock`.

## 2.2 Duration class

`template <class Rep, class Period = ratio<1> >class duration;`
A duration object expresses a time span by means of a **count** (stored as an object of member type `rep`) and a **period**.




```
template<
    class Rep,
    class Period = std::ratio<1>
> class duration;
```
Consists of the type of the tick `Rep` and the length of a tick `Period`.

1. Rep: an arithmetic type representing the number of ticks
2. Period: a `std::ratio` representing the tick period (number of second's fractions per tick)
- `std::ratio<1,1>` stands for a second.
- `std::ratio<1,60>` is a minute.
- `std::ratio<1,1000>` is a millisecond.

Predefined durations:

```
std::chrono::nanoseconds = typedef duration<signed int, std::nano> nanoseconds;
std::chrono::microseconds = typedef duration<signed int, std::micro> microseconds;
std::chrono::hours = typedef duration<signed int, ratio<3600>> hours;
```



## 2.3. std::chrono::time_point_cast

Examples:

`std::chrono::system_clock::now()` returns `std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>`




Different durations are not implicitly convertible to one another, You can have to convert time points with different durations using `std::chrono::time_point_cast`

```cpp
std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timePoint = 
	std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
```		  

Refs: [1](https://stackoverflow.com/questions/15777073/how-do-you-print-a-c11-time-point), [2](https://www.modernescpp.com/index.php/time-duration), 
[3](https://en.cppreference.com/w/cpp/chrono/duration)


## 2.4.  std::chrono::duration_cast

```cpp
const auto now = std::chrono::system_clock::now();
time_t time = std::chrono::system_clock::to_time_t(now);
std::cout << time << "\n";
std::cout << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() << "\n";
std::cout << std::chrono::system_clock::from_time_t(time).time_since_epoch().count() << "\n";
std::cout << std::chrono::system_clock::time_point(std::chrono::seconds(time)).time_since_epoch().count() << "\n";
```

Refs: [1](https://stackoverflow.com/questions/66346389/stdchronosystem-clock-and-c-time), [2](https://en.cppreference.com/w/cpp/chrono/duration/duration_cast)
,[3](https://stackoverflow.com/questions/51538022/how-the-time-point-created-with-different-durationstdchronomilliseconds-and)

[src](../src/date_time.cpp)