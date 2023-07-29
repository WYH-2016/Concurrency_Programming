#include<iostream>
#include<thread>

void f()
{
    std::cout << std::this_thread::get_id() << ": hello concurrency world\n";
}

int main()
{
    std::thread t{ f };
    t.join();
}
