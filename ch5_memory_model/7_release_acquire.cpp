// memory_order_seq_cst makes there only one consistent sequence, but take too much resource
// memory_order_release combinate with memory_order_acquire can provide synchronizeation between threads
// it won't take too much resources to make a global consistent sequnce
// only make sure that:
//  -In the same thread, operates before memory_order_release won't be rearranged after it
//  -In the same thread, operate after memory_order_acquire won't be rearranged before it

// =====================================================================================================
// test 1
// =====================================================================================================
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x()
{
    x.store( true, std::meomry_order_release );     // 1
}
void write_y()
{
    y.store( true, std::meomry_order_release );     // 2
}
void read_x_then_y()
{
    while( !x.load( std::memory_order_acquire ) );  // 3 make sure 3 pass after 1, but independently of 2
    if( !y.load( std::memory_order_acquire ) )      // 5 so 5 may load y value is false
        z++;
}
void read_y_then_x()
{
    while( !y.load( std::memory_order_acquire ) );  // 4 make sure 4 pass after 2, but independently of 1
    if( !x.load( std::memory_order_acquire ) )      // 6 so 6 may load x value is false
        z++;
}

int main()
{
    x = false;
    y = false;
    z = 0;

    std::thread a( write_x );
    std::thread b( write_y );
    std::thread c( read_x_then_y );
    std::thread d( read_y_then_x );
    a.join();
    b.join();
    c.join();
    d.join();
    assert( z.load() != 0 );                        // assert will not pass
}

// =====================================================================================================
// test 2
// =====================================================================================================

#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y()
{
    x.store( true, std::memory_order_relaxed );     // 1 
    y.store( true, std::memory_order_release );     // 2 happen after 1
}
void read_y_then_x()
{
    while( !y.load( std::memory_order_acquire ) );  // 3 synchoronized with 2, make sure load values 2 store
    if( !x.load( std::memory_order_relaxed ) )      // 4 happen after 3, then 1 happen before 4
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
    assert( z.load() != 0 );                        // 5 the assert will pass
}
