# quick notes
TODO


DOING

DONE
- ensure sync issue resolves => currently the timer program & actual worker function is executing in a non-deterministic manner
    - resovled this by having a deterministic timer that ends by deadline
- ensure timer is a plug & playable module => now the timer logic and worker logic is coupled together; i want an easy to import & use timer that wouldnt need the user to look too much into the implementation
- write a 5 sec timer, which alerts when time ends


## -----------
# implemting accurate time

1 issue is that the program requires us to start the `worker` task first before sleeping for a `duration`
issue: there's a gap between starting the worker & starting the timer; worker might run for `total_duration` = `duration` + `gap` time

bad example:
```cpp
void run_for(int duration_sec, std::function<void()> worker_function, std::string task_name) {
    // create worker thread to run for 10s
    std::thread worker_thread{worker_function, 10};
    // run in the background??
    worker_thread.detach();

    // start the timer
    std::cout << "Timer started for: " << task_name << "," << duration_sec << "s..." << std::endl;
    
    // reference this thread
    std::this_thread::sleep_for(std::chrono::seconds(duration_sec));
    
    // end the timer
    std::cout << "Timer end! your " << duration_sec << "s is up for: " << task_name << std::endl; //  << worker_function.name => cpp dont store
}
```

## solution: deadline pattern

instead of timing the duration, calculate the `deadline` after a specified `start_time` & run till `deadline`

```cpp
void run_for(int duration_sec, std::function<void()> worker_function, std::string task_name) {
    auto deadline = std::chrono::steady_clock::now() + std::chrone::seconds(duration_sec); // calc a deadline time from now; use auto for chrone types => v. verbose
    std::thread worker_thread{worker_function, 10} // start the worker function in a separate thread
    worker_thread.detach()
    std::this_thread::sleep_until(deadline);
    std::cout << "Timer ends! duration of " << duration_sec << "s is up for " << task_name << std::endl;
}
```
* in general, threads killing a thread from the outside is unsafe (leaked resources etc.)
    - could use a flag, but will need to change the implementation of the worker_function => couples timer & worker_function logic
    - another way is to use processes instead of threads, then force kill a process => overhead in creating process vs threads
* a better way is to return status of whether it completed within the timer and let the program run till end of main => then OS will clean up all the resources
    - but means thread will continue running till the end of the `main()` function outside of `run_for`

### return status

the above `run_for` doesnt return a status and even when the function ends, `worker_function` still continues running till the end of `main()` (not run_for)
we need to check the status, by wrapping the thread in a future task, & track the status at the end of deadline to let the user handle this

```cpp
void run_for(int duration_sec, std::function<void()> worker_function, std::string task_name) {
    auto deadline = std::chrono::steady_clock::now() + std::chrone::seconds(duration_sec);
    
    // wrap in future
    std::packaged_task<void(int)> task{worker_function}; // create a packaged_task (promise), where we can monitor this task's execution; task's function returns void, & takes in int
    std::future<void> future_res = task.get_future();
    
    std::thread worker_thread{std::move(task), 10}; // task is executed in the worker thread, with arg = 10
    worker_thread.detach(); // either call join() or detach() to ensure our thread does crash the program (via std::terminate)

    auto status = future_res.wait_for(std::chrono::seconds(duration_sec));

    std::cout << "Timer ends! duration of " << duration_sec << "s is up for " << task_name << std::endl;
    if (status == std::future_status::ready) {
        std::cout << "PASSED: Task finished before timer!" << std::endl
        return true;
    } else {
        std::cout << "FAILED: Task is still running!" << std::endl
        return false
    }
}
```
- `worker_thread.detach();`
    - either call join() or detach() to ensure our thread does crash the program (via std::terminate)
    - note that if an object is destroyed while it is still 'joinable' the program will crash
- `std::packaged_task`
    - is a wrapper that contains both `std::promise` & the function you want to run
    - helps you to call the `set_value` or `set_exception` when the worker function ends

### Promises & futures
* std::promise allows 1 thread to fulfill a promise by setting a value or an exception that will be access by another thread at a later time
    - worker thread -> producer
    - main thread -> consumer
`get_future` - returns a std::future associated with the promise
    - `set_value()` - stores a value & makes the future ready
    - `set_exception()` - stores an exception to be thrown in the future thread
```cpp
#include <iostream>
#include <thread>
#include <future>

void producer(std::promise<int>&& prom) {
    // Fulfill the promise
    prom.set_value(42); 
}

int main() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future(); // set fut to contain the future value of prom (the promise)

    std::thread t{producer, std::move(prom)}; // move this promise from the current `main` thread's local MEM, to the bg thread's local MEM
    // promises are non-copyable -> its a unique communication channel, cant have 2 copies of the same promise => hence need to move
    // producer is called inside the bg thread, & takes in the argument prom => value gets set inside the fut

    // Retrieve value from future
    std::cout << "Result: " << fut.get() << std::endl; 
    
    t.join();
    return 0;
}
```