#include <mutex>
#include <memory>

std::share_ptr<some_resource> resource_ptr;
std::once_flag resource_flag;

void init_resource()
{
    resource_ptr.reset( new some_resource );
}

void foo()
{
    std::call_once( resource_flag, init_resource ); // how to pass parameters?
    resource_ptr->do_something();
}

// class member initial using " call_once "

class X
{
private:
    connection_info connection_details;
    connection_handle connection;
    std::once_flag connection_init_flag;

    void open_connection()
    {
        coonection = connection_manager.open( connection_details );
    }

public:
    X( connection_info const & connection_details_ ):
        connection_details( connection_details_ )
    {}

    void send_data( data_packet const & data )
    {
        std::call_once( std::once_flag, &x::open_connection, this); // "this" is &x::open_connection's fst para
        return connection.send_data( data );
    }
    
    void recv_data( data_packet const & data )
    {
        std::call_once( std::once_flag, &x::open_connection, this);
        return connection.recv_data( data );
    }
    
};
