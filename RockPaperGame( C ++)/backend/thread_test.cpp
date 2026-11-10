#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

int main()
{
    std::thread t([]() {
        std::cout << "Threads are working!" << std::endl;
    });

    t.join();
    return 0;
}