# Date and Time in C
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

# Date and Time in C++

`std::chrono::time_point`

`class system_clock`: It is the only C++ clock that has the ability to map its time points to C-style time

[src](../src/date_time.cpp)