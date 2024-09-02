# spdlog

Refs: [1](https://github.com/gabime/spdlog)


### How to enable/disable spdlog logging before compilation

You can disable all logging before you compile the code by adding the following macro (before including spdlog.h):
```
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#include<spdlog.h>
```

Refs: [1](https://stackoverflow.com/questions/45621996/how-to-enable-disable-spdlog-logging-in-code), [2](https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/spdlog.h)

