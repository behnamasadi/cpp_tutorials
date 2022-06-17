# System Call
system call is the programmatic way in which a computer program requests a service from the kernel of the operating system to be executed

```cpp
#include <string>

std::string cmd="mkdir foo";
system(cmd.c_str());
}

```
or you can call `mkdir` directly"

```cpp
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

mkdir("foo", 0755);
```

Linux System Call Table: [Here](https://thevivekpandey.github.io/posts/2017-09-25-linux-system-calls.html)



