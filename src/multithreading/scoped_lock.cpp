/*
It differs from a lock guard in that it is a wrapper for not one, but multiple mutexes.

This can be useful when one deals with multiple mutexes in a single scope. One reason to
use a scoped lock is to avoid accidentally introducing deadlocks and other unpleasant
complications with, for example, one mutex being locked by the scoped lock, another lock
still being waited upon, and another thread instance having the exactly opposite situation.
One property of a scoped lock is that it tries to avoid such a situation, theoretically making
this type of lock deadlock-safe.
*/

int main()
{
}
