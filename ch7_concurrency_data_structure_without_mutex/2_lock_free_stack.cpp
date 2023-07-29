// Three level concurrency of functions:
//  - obstruction free: if other thread all PAUSED, the only RUNNING thread will finish in limited steps.
//  - lock free: multi-thread operate data structure, there must have a thread can finish in limited steps.
//  - wait free: multi-thread operate data structure, all thread can finish in limited steps.

// there is a Lock-Free push() for stack

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

public:
    void push( T const& data )
    {
        node* const new_node = new node( data );
        new_node->next = head.load();
        while( !head.compare_exchange_weak( new_node->next, new_node ) );   // use compare_exchange_weak, if head != new_node->next, then assign head to new_node->next; else assign new_node to head
    }

    std::shared_ptr<T> pop()
    {
        node* old_head = head.load();
        while( old_head &&
               !head.compare_exchange_weak( old_head, old_head->next ) );

        return old_head ? old_head->data : std::shared_tr<T>();
    }
};
