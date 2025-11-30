#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

void worker(std::ostream &os, int id)
{
    std::osyncstream sync_os(os);
    sync_os << "Worker " << id << " is starting\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sync_os << "Worker " << id << " is done\n";
}

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 16; ++i)
    {
        threads.emplace_back(worker, std::ref(std::cout), i);
    }

    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}