// TODO: write a 5 sec timer, which alerts when time ends

#include <iostream>  // for std::cout
#include <thread>  // for threading
#include <chrono> // for time?

void sleep_wait(int duration) {
    // empty task that needs to be done => 
    std::cout << "Start 10s task..." << std::endl;
    // sleep for 10s
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "10s task ended!" << std::endl;
}

int main() {
    // set a timer
    // do_something
    // timer ends & alerts
    // finishes program even before do_something can end

    // run 10s task in separate detached thread
    // 5 second timer will be here in main thread
    std::thread worker_thread{sleep_wait, 10};

    worker_thread.detach();

    std::cout << "5 second timer started..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "5 second timer ended! closing all other unfinished threads" << std::endl;
}

void busy_wait() {
    // TODO:
    // empty task that needs to be done => CPU heavy
}