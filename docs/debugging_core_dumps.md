# Core Dump
A core dump is a file containing a process's memory when the process crashes or terminates unexpectedly. First you need to enable it.
`ulimit` is Linux shell command that can set or report the resource limit of the current user, if you type
`ulimit -c` it will show you the maximum size of core files created, or `ulimit -s` will show the maximum stack size or just type
`ulimit -a` o get all resource limitation.

# Enabling Core Dump Files
Type `ulimit  -c unlimited` to set the core dump files unlimited. Ok lets create a program that purposely crash:


```cpp
unsigned char  *x=new unsigned char [5];
for(int i=0;i<10000000;i++)
{
	x[i]++;
}
```
now if you run this your program will crash and on Ubuntu, core files are piped to Apport (Ubuntu's crash reporting system) 
and the dump file will be stored at `/var/lib/systemd/coredump/`. The detail of file can seen with `cat /proc/sys/kernel/core_pattern`

You can use `coredumpctl list` to check the dumped files and `coredumpctl info <your-binary>` to see information about your file

`coredumpctl gdb sandbox`


`gdb sandbox <<your-dump-file>>`


`kill -SEGV <PID>`

`kill -SIGSEGV <PID>`

`sudo service apport stop`

`ps ax | grep firefox`


`kill -9 pid`


Refs: [1](https://doc.qt.io/qtcreator/creator-debugger-operating-modes.html)
