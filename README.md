# Cpp timer

0. [DONE]

write a 5 sec timer, which alerts & ends main thread when time ends (even if other busy threads are still alive)

1. (issue)

currently there are many occasions where 5s timer starts b4 sleep_wait executed
this is not intended as we executed the timer first

likely reasons of failure:
    a. concurrency/ race condition
    b. std::cout's buffer

=> resolve this issue!

2. heavy CPU implementation

we want to also be able to create a timer for busy task; implent busy_wait below
(we will need an alert to context switch out of the busy task)
