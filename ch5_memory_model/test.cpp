#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> b;


void foo()
{
    std::cout << " Thread begin\n";
    std::this_thread::sleep_for( std::chrono::seconds( 5 ));
    b.store( false );

    std::cout << " Thread end\n";
}

int main()
{
    b.store( true );
    bool expected = false;
    
    std::thread t( foo );
    //std::cout << "return: " << b.compare_exchange_weak( expected, true ) << "\n";
    //std::cout << expected << "\n";
    
    // Follow while go on until b value is false, then set it true and break, like a mutex.lock
    while( !b.compare_exchange_weak( expected, true) || expected );
    
    t.join();
    std::cout << "While get break \n";
}
