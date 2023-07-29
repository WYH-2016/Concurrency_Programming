// As a fundamental data structure, list is usful extremly threadsafe list
// If we want effective concurrency, let each node has its own mutex
// Be careful of deadlock, functions given by user ...

#include <mutex>
#include <memory>

template< typename T >
class threadsafe_list
{
    struct node
    {
        std::mutex m;
        std::shared_ptr< T > data;
        std::unique_ptr< node > next;

        node() {}
        node( T const& _data):data( std::make_shared< T >(_data) ) {}
    };

    node head;

public:
    threadsafe_list(){}

    threadsafe_list( threadsafe const & ) = delete;

    ~threadsafe_list()
    {
        remove_if([](node const &){ return true;} )
    }

    threadsafe_list& operator=( threadsafe_list const & ) = delete;

    void push_front( T const& value)
    {
        std::unique_lock<node> new_node( new node(value) );
        std::lock_guard<std::mutex> lk( head.m );
        new_node->next = std::move( head.next );
        head.next = std::move( new_node );
    }

    template< typename Function >
    void for_each( Function f )
    {
        node* current = &head;
        std::unique_lock<std::muex> lk( head.m );
        while( node* const next = current->next.get() )
        {
            std::unique_lock<std::mutex> next_lk( next->m );
            lk.unlock();
            f( *next->date );   // value pass ?
            current = next;
            lk = std::move( next_lk );
        }
    }

    template< typename Predicate >
    std::shared_ptr<T> find_first_if( Predicate p )
    {
        node* current = &head;
        std::unique_lock<std::muex> lk( head.m );
        while( node* const next = current->next.get() )
        {
            std::unique_lock<std::mutex> next_lk( next->m );
            lk.unlock();
            if( p( *next->data ) )
            {
                return next->data;
            }
            current = next;
            lk = std::move( next_lk );
        }
    }

    template< typename Predicate >
    void remove_if( Predicate p )
    {
        node* current = &head;
        std::unique_lock<std::muex> lk( head.m );
        while( node* const next = current->next.get() )
        {
            std::unique_lock<std::mutex> next_lk( next->m );
            if( p( *next->data ) )
            {
                std::unique_ptr<node> old_next = std::move( current->next );
                current->next = std::move( next->next );
                next_lk.unlock();
            }   // old_node destruct here, after mutex unlock! 
                // But we get current locked, no thread can get old_node without get currrent.m.
                // So this operate is thread safe!
            else
            {
                lk.unlock();
                current = next;
                lk = std::move( next_lk );
            }
        }
    }
};
