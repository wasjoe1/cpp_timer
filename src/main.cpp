#include <iostream>  // for std::cout
#include <thread>  // for threading
#include <chrono> // for std::chrono
#include <functional> // for std::function
#include <future> // for std::future & std::packaged_task
#include <ctime> // for time conversion to string


void sleep_wait(int duration) {
    std::cout << "Start sleep_wait(10s) ..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "sleep_wait function ended!" << std::endl;
}

bool run_for(int duration_sec, std::function<void(int)> worker_function, std::string task_name) {
    auto now = std::chrono::steady_clock::now();
    auto deadline = now + std::chrono::seconds(duration_sec);
    std::cout << "Timer started... (start: " << now.time_since_epoch().count() << ", end: " << deadline.time_since_epoch().count() << ")" <<  std::endl;
    
    // wrap worker in a promise
    std::packaged_task<void(int)> task{worker_function};
    std::future<void> future_res = task.get_future();
    
    // run it in a bg thread
    std::thread worker_thread{std::move(task), 10};
    worker_thread.detach();

    // wait till 
    auto status = future_res.wait_until(deadline);

    std::cout << "Timer ends! duration of " << duration_sec << "s is up for " << task_name << std::endl;
    if (status == std::future_status::ready) {
        std::cout << "PASSED: Task finished before timer!" << std::endl;
        return true;
    } else {
        std::cout << "FAILED: Task is still running!" << std::endl;
        return false;
    }
}


int main() {
    // run the sleep_wait function for 5s
    bool is_success = run_for(5, sleep_wait, "sleep_wait");
}