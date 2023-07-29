// memory_order_relaxed makes things happends in a different way
// there still certain rules

#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> x( 0 ), y( 0 ), z( 0 );                    // 1
std::atomic<bool> go( false );                              // 2
unsigned const loop_count = 10;

struct read_values
{
    int x, y, z;
};

read_values values1[ loop_count ];
read_values values2[ loop_count ];
read_values values3[ loop_count ];
read_values values4[ loop_count ];
read_values values5[ loop_count ];

void increment( std::atomic<int>* var_to_inc, read_values* values )
{
    while( !go )                                            // 3 - wait flag
        std::this_thread::yield();

    for( unsigned i = 0; i < loop_count; i++)
    {
        values[i].x = x.load( std::memory_order_relaxed );
        values[i].y = y.load( std::memory_order_relaxed );
        values[i].z = z.load( std::memory_order_relaxed );
        var_to_inc->store( i+1, std::memory_order_relaxed );// 4 - increse value
        std::this_thread::yield();
    }
}
void read_vals( read_values* values )
{
    while( !go )                                            // 5 - wait flag
        std::this_thread::yield();
    for( unsigned i = 0; i<loop_count; i++)
    {
        values[i].x = x.load( std::memory_order_relaxed );
        values[i].y = y.load( std::memory_order_relaxed );
        values[i].z = z.load( std::memory_order_relaxed );
        std::this_thread::yield();
    }
}
void print( read_values* v )
{
    for( unsigned i = 0; i < loop_count; i++)
    {
        if( i )
            std::cout << ",";
        std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
    }
    std::cout << std::endl;
}

int main()
{
    std::thread t1( increment, &x, values1);
    std::thread t2( increment, &y, values2);
    std::thread t3( increment, &z, values3);
    std::thread t4( read_vals, values4 );
    std::thread t5( read_vals, values5 );
    go = true;                                              // 6 - flag
    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();
    print( values1 );                                       // 7 - print values
    print( values2 );
    print( values3 );
    print( values4 );
    print( values5 );
}
// check the print output, You will find:
//  - every incement-thread will see their own value change as soon as it happens
//  - when load values changed by other thread, it won't get the latest value
//  - when load values changed by other thread, it will follow a change list, once you get a newer value, next time load won't go back for a older one;
