// memory_order_relaxed gives no concurrency surence
// there is no sequencial relations between threads

#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x,y;
std::atomic<int> z;

void write_x_then_y()
{
    x.store( true, std::memory_order_relaxed );     // 1
    y.store( true, std::memory_order_relaxed );     // 2
}
void read_y_then_x()
{
    while( !y.load( std::memory_order_relaxed ));   // 3
    if( !x.load( std::memory_order_relaxed ))       // 4
        z++;
}

int main()
{
    x = false;
    y = false;
    z = 0;

    std::thread a( write_x_then_y );
    std::thread b( read_y_then_x );
    a.join();
    b.join();

    assert( z.load() != 0 );                        // 5 - not pass
}

// although operates (memory_order_relaxed) in same thread will follow a consistent sequence depends on codes sequence
// in thread a: the x value set true first and y value will change then
// in thread b: 'while' go until y value change true, but 'if' load x value may still get false
