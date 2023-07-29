#include <mutex>
#include <condition_variable>
#include <memory>

template< typename T >
class threadsafe_queue
{
private:
    struct node{
        std::shared_ptr< T > data;
        std::unique_ptr< node > next;
    };
    std::mutex m_head_mutex, m_tail_mutex;
    std::unique_ptr< node > head;
    node* tail;
    std::condition_variable m_data_cond;

private:
    node* get_tail()
    {
        std::lock_guard<std::mutex> lk( m_tail_mutex );
        return tail;
    }

    std::unique_ptr< node > pop_head()
    {
        std::unique_ptr< node > old_head = std::move( head );
        head = std::move( old_head -> next );
        return old_head;
    }

    std::unique_lock< std::mutex > wait_for_data()
    {
        std::unique_lock< std::mutex > haed_lock( m_head_mutex );
        m_data_cond.wait( head_lock, [&]{ return head.get() != get_tail(); } )
        return std::move( head_lock );
    }

    std::unique_ptr< node > wait_pop_head()
    {
        std::unique_lock< std::mutex > head_lock( wait_for_data() );
        return pop_head();
    }

    std::unique_ptr< node > wait_pop_head( T& value )
    {
        std::unique_lock< std::mutex > head_lock( wait_for_data() );
        value = std::move( *head->data );
        return pop_head();
    }

    std::unique_ptr< node > try_pop_head()
    {
        std::lock_guard< std::mutex > head_lock( m_head_mutex );
        if( head.get == get_tail() )
        {
            return std::shared_ptr<node>();
        }
        return pop_head();
    }

    std::unique_ptr< node > try_pop_head( T& value )
    {
        std::lock_guard< std::mutex > head_lock( m_head_mutex );
        if( head.get == get_tail() )
        {
            return std::shared_ptr<node>();
        }
        value = std::mopve( *head->data );
        return pop_head();
    }

public:
    threadsafe_queue():
        head( new node ), tail( head.get() )
    {}
    threadsafe_queue( const threadsafe_queue & ) = delete;
    threadsafe_queue& operater=( const threadsafe_queue & ) = delete;

    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr< node > old_head = try_pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    bool try_pop( T& value )
    {
        std::unique_ptr< node > old_head = try_pop_head( value );
        return old_head;
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_ptr< node > const old_head = wait_pop_head();
        return head->data;
    }

    void wait_and_pop( T& value )
    {
        std::unique_ptr< node > const old_head = wait_pop_head( value );
    }

    void push( T new_value )
    {
        std::shared_ptr<T> new_data( std::make_shared<T>( std::move( new_value ) ) );
        std::unique_ptr< node > p( new node );

        {
            std::unique_lock< std::mutex > tail_lock( m_tail_mutex );
            tail->data = new_data;
            node* const new_tail = p.get();
            tail->next = std::move( p );
            tail = new_tail;
        }

        m_data_cond.notify_one();
    }

    bool empty()
    {
        std::unique_lock< std::mutex > head_lock( m_head_mutex );
        return ( head.get() == tail_get() );
    }
};
