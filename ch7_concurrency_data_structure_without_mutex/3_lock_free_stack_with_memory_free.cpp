// In 2_lock_free_stack.cpp, we ignore a important thing -- reclaiming memory
// For function push(), there is no memory problem, we make Heap Memory for data to be shared_ptr
// But int function pop(), threads must get Head_Node's data and free it
// then, there might more than one thread get the same Head_Node, but one thread go on: Get data, let head pointer point to old Head_Node->next and free the old Head_Node
// after that, all other threads go to a Nullptr

template< typename T >
class lock_free_stack
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        node* next;
        node( T const& _data): data( std::make_shared<T>( _data ) )
        {}
    };

    std::atomic< node* > head;

    std::atomic< unsigned > threads_in_pop;  // atomic unsigned to count threads in pop()
    std::atomic< node* > to_be_deleted;

    static void delete_nodes( node* nodes )
    {
        while( nodes )
        {
            node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

    void try_reclaim( node* old_head )
    {
        if( threads_in_pop == 1 )
        {
            node* nodes_to_delete = to_be_deleted.exchange( nullptr );
            if( !--thread_in_pop )
            {
                delete_nodes( nodes_to_delete );
            }
            else if( nodes_to_delete )
            {
                chain_pending_nodes( nodes_to_delete );
            }
            delete old_head;
        }
        else
        {
            chain_pending_node( old_head );
            --threads_in_pop;
        }
    }

    void chain_pending_nodes( node* nodes )
    {
        node* last = nodes;
        while( node* const next = last->next )
        {
            last = next;
        }
        chain_pending_nodes( nodes, last );
    }

    void chain_pending_nodes( node* first, node* last )
    {
        last->next = to_be_deleted;
        while( !to_be_deleted.compare_exchange_weak( last->next, first );
    }

    void chain_pending_node( node* n )
    {
        chain_pending_nodes( n, n );
    }

public:
    void push( T const& data )
    {
        node* const new_node = new node( data );
        new_node->next = head.load();
        while( !head.compare_exchange_weak( new_node->next, new_node ) );   // use compare_exchange_weak, if head != new_node->next, then assign head to new_node->next; else assign new_node to head
    }

    std::shared_ptr<T> pop()
    {

        ++threads_in_pop;

        node* old_head = head.load();
        while( old_head &&
               !head.compare_exchange_weak( old_head, old_head->next ) );
        
        std::shared_ptr< T > res;
        if( old_head )
        {
            res.swap( old_head->data );
        }
        try_reclaim( old_head );
        return res;
    }
};
