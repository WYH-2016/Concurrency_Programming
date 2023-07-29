// There SIX types of memory order
//  - memory_order_relaxed
//  - memory_order_consume
//  - memory_order_acquire
//  - memory_order_release
//  - memory_order_acq_rel
//  - memory_order_seq_cst
// They can be devide into THREE categories
//  - sequentially consistent ( memory_order_seq_cst )
//  - acquire & release ( memory_order_consume, memory_order_acquire, memory_order_release, memory_order_acq_rel)
//  - relaxed ( memory_order_relaxed )

// memory_order_seq_cst will make sure there is only one consistent global operator sequense

#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x()
{
    x.store( true, std::memory_order_seq_cst );
}
void write_y()
{
    y.store( true, std::memory_order_seq_cst );
}

void read_x_then_y()
{
    while( !x.load( std::memory_order_seq_cst ));
    if( y.load( std::memory_order_seq_cst ))
        z++;
}
void read_y_then_x()
{
    while( !y.load( std::memory_order_seq_cst ));
    if( x.load( std::memory_order_seq_cst ))
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
    assert( z.load() != 0 );    // this assert will pass!
}
// because all atomic operate set memory_order_seq_cst
// it means all thread will follow a consistent memory order
// although it's on depend that thread a(write_x) or thread b(write_y) will run first
// but thread c and thread d will follow the same sequence
// if write_x run first, the 'if' in read_x_then_y may be fasle, but 'if' in read_y_then_x must be true
// it as same if write_y run first
