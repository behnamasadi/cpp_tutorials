# Thread design pattern

## Thread Pool (Boos and Worker/ Replicated Workers / Worker-crew Model)
One benefit of a thread pool over creating a new thread for each task is that thread creation and destruction 
overhead is restricted to the initial creation of the pool, which may result in better performance and better 
system stability. Creating and destroying a thread and its associated resources can be an expensive
 process in terms of time. An excessive number of threads in reserve, however, wastes memory,
 and context-switching between the runnable threads invokes performance penalties.
 

