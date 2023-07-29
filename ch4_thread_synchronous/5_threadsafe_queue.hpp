// thread safe queue
// normal queue templace container has follower member function
// - size() & empty() for query states
// - front() & back() for get items 
// - push() &  pop() &  emplace() for modify item
// these interfaces have conditional rations, must be changed for thread safe
// combin front() and pop() as try_pop() or wait_and_pop()

#include <queue>
#include <memory>
#include <mutex>
#include <Rcondition_variable>

template<typename T>
class ThreadSafeQueue
{
private:
    mutable std::mutex m_mutex;
    std::queue<T> m_data_queue;
    std::condition_variable m_data_cond;

public:
    ThreadSafeQueue()
    {}

    ThreadSafeQueue( ThreadSafeQueue const& other )
    {
        std::lock_guard<std::mutex> lk( other.m_mutex );
        m_data_queue = other.m_data_queue;
    }

    void push( T new_value )
    {
        std::lock_guard<std::mutex> lk( m_mutex );
        m_data_queue.push( new_data );
        m_data_cond.notify_one();
    }

    void wait_and_pop( T& value )
    {
        std::unique_lock<std::mutex> lk( m_mutex );
        m_data_cond.wait( lk, [this]{ return !m_data_queue.empty(); } );
        value = m_data_queue.front();
        m_data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk( m_mutex );
        m_data_cond.wait( lk, [this]{ return !m_data_queue.empty(); } );
        std::shared_ptr<T> res( std::make_shared<T>( m_data_queue.front() ) );
        m_data_queue.pop();
        return res;
    }

    bool try_pop( T& value )
    {
        std::lock_guard<std::mutex> lk( m_mutex );
        if( m_data_queue.empty() ) { return false; }

        value = m_data_queue.front();
        m_data_queue.pop();

        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk( m_mutex );
        if( m_data_queue.empty() ) { return std::shared_ptr<T>(); }

        std::shred_ptr<T> res( std::make_shared<T>( m_data_queue.front() ) );
        m_data_queue.pop();

        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk( m_mutex );
        return m_data_queue.empty();
    }
};
