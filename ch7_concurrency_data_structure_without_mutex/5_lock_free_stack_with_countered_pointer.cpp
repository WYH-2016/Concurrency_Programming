// For delete nodes which no thread point to, we can use countered pointer
// it looks like std::shared_ptr, but we should know that std::shared_ptr is not lock_free
// if the platform we use let std::atomic_is_lock_free( &some_shared_ptr ) return true, it's easy to reclaim memory
// if not, we can use separate point counter to realize countered_pointer

template< typename T >
class lock_free_stack
{
private:
    struct node;
    struct counted_node_ptr
    {
        int external_count;
        node* ptr;
    };

    struct node
    {
        std::shared_ptr<T> data;
        std::atomic<int> internal_count;
        couted_node_ptr next;

        node( T const& _data ):
            data( std::make_shared<T>(_data)),
            internal_count(0)
        {}
    };

    std::atomic<counted_node_ptr> head;

    void increase_head_count( counted_node_ptr& old_counter )
    {
        counted_node_ptr new_counter;
        do
        {
            new_counter = old_counter;
            ++new_counter->external_counter;
        }while( !head.compare_exchange_strong(old_counter, new_counter));

        /*
        // more detail memory consequence
        while( !head.compare_exchange_strong( old_counter, new_counter,
                                            std::memory_order_acquire, std::memory_order_relaxed) );
        // compare_exchange_weak return true - std::memory_order_release
        //                       return false - std::memory_order_relaxed
        */
        old_counter.external_count = new_counter.external_count;
    }

public:
    ~lock_free_stack()
    {
        while( pop() );
    }

    void push( T const& data )
    {
        counted_node_ptr new_node;
        new_node.ptr = new node(data);
        new_node.external_count = 1;
        new_node.ptr->next = head.load();
        /*
        // more detail memory consequence
        new_node.ptr->next = head.load( std::memory_order_relaxed );
        */

        while(!head.compare_exchange_weak( new_node.ptr->next, new_node );
        /*
        // more detail memory consequence
        while( !head.compare_exchange_weak( new_node.ptr->next, new_node,
                                            std::memory_order_release, std::memory_order_relaxed) );
        // compare_exchange_weak return true - std::memory_order_release
        //                       return false - std::memory_order_relaxed
        */
    }

    std::shared_ptr<T> pop()
    {
        counted_node_ptr old_head = head.load();

        for(;;)
        {
            increase_head_count( old_head );
            node* counst ptr = old_head.ptr;
            if(!ptr)
            {
                return std::shared_ptr<T>();
            }
            if( head.compare_exchange_strong(old_head, ptr->next))
            {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                int const count_increase = old_jhead.external_count - 2;
                if( ptr->internal_count.fetch_add(counter_increase) == -counter_increase )
                {
                    delete ptr;
                }
                return res;
            }
            else if( ptr->internal_count.fetch_sub(1) == 1 )
            {
                delete ptr;
            }
        }
    }
};

