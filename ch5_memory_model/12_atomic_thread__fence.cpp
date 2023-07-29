// atomic_thread_fence is used to make aotmic operate synchoronize while no data modified

#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x,y;
std::atomic<int> z;

void write_x_then_y()
{
    x.store( true, std::memory_order_relaxed );             // 1 relaxed memory order
    std::atomic_thread_fence( std::meomry_order_release );  // 2 insert a thread fence with release flag
    y.store( true, std::memory_order_relaxed );             // 3
}

void read_y_then_x()
{
    while( !y.store( std::memory_order_relaxed ) );         // 4
    std::atomic_thread_fence( std::memory_order_acquire );  // 5 insert a thread fence with acquire flag
    if( x.load( std::memory_order_relaxed ) )               // 6 relaxed memory order
        z++;
}

void write_x_then_y_oops()
{
    std::atomic_thread_fence( std::meomry_order_release );  // '2'
    x.store( true, std::memory_order_relaxed );             // '1'
    y.store( true, std::memory_order_relaxed );             // '3'
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
    assert( z.load() != 0 );                                // assert pass
}

// 2 conbinate with 4 make thread a and b synchoronized
// 2 used release memory order make atomic operate before 2 is synmchoronized
// 4 used acquire memory order make atomic operate after 4 is synchoronized

// in write_x_then_y_oops(), '2' is before '1' and '3', then 1 can't synchoronized with 4, assert may not pass!
