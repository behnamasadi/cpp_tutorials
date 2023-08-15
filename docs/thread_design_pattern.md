# Concurrency Design Pattern and Principles


## Concurrency Patterns
Refs: [1](https://www.dre.vanderbilt.edu/~schmidt/POSA/POSA2/conc-patterns.html), [2](https://en.wikipedia.org/wiki/Concurrency_pattern)


## Reactor pattern
Refs: [1](https://en.wikipedia.org/wiki/Reactor_pattern)

## Event Loop
Refs: [1](https://habr.com/en/articles/665730/), [2](https://en.wikipedia.org/wiki/Event_loop)

## Thread Pool (Boos and Worker/ Replicated Workers / Worker-crew Model)
One benefit of a thread pool over creating a new thread for each task is that thread creation and destruction 
overhead is restricted to the initial creation of the pool, which may result in better performance and better 
system stability. Creating and destroying a thread and its associated resources can be an expensive
 process in terms of time. An excessive number of threads in reserve, however, wastes memory,
 and context-switching between the runnable threads invokes performance penalties.
 Refs: [1](https://en.wikipedia.org/wiki/Concurrency_pattern), [2](https://www.baeldung.com/concurrency-principles-patterns), [3](https://www.dre.vanderbilt.edu/~schmidt/POSA/POSA2/conc-patterns.html)
 
 
 


 

