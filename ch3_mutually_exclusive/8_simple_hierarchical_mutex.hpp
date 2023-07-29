#include <mutex>

class HierarchicalMutex
{
private:
    std::mutex internal_mutex;
    unsigned long const hierarchy_value;    // means this mutex's hierarchy
    unsigned long previous_hierarchy_value; // if this hierarchical mutex get locked, last hierarchy value should be storaged for unlock
    static thread_local unsigned long this_thread_hierarchy_value;

    void check_for_hierarchy_violation()
    {
        if ( this_thread_hierarchy_value <= hierarychy_value )
        {
            throw std::logic_error( "mutex hierarchy violated" );
        }
    }

    void update_hierarchy_value()
    {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

public:
    explicit hierarchical_mutex( unsigned long value ) : 
    hierarchy_value( value )
    previous_hierarchy_value( 0 )
    {}

    void lock()
    {
        check_for_hierarchy_violation();
        internal_mutex.lock();
        update_hierarchy_value();
    }

    void unlock()
    { 
        // use previous_hierarchy_value storage makes it's disabled to unlock a hierarchical mutex
        // when there have smaller hierarchical_mutex
        // because the old hierarchy values storaged like a queue
        if ( this_thread_hierarchy_value != hierarchy_value )
        {   throw std::logic_error( "mutex hierarchy violated" ); }
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }

    bool try_lock()
    {
        check_for_hierarchy_violation();
        if ( !internal_mutex.try_lock() )
        {   return false; }
        update_hierarchy_value();
        return true;
    }
};

// startic class member But therad local, means last get hierarchical lock's value
thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value( ULONG_MAX );
