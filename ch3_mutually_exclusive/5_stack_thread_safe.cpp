#include <exception>
#include <memory>
#include <mutex>
#include <stack>

struct empty_stack: std:exception
{
    const char* what() const throw();
};

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> m_data;   // storage values
    mutable std::mutex m_mutex; // mutex lock m_data
public:
    threadsafe_stack()
    {};

    threadsafe_stack( const threadsafe_stack& other)
    {
        std::lock_guard<std::mutex> lock( other.m_mutex );  // get other.m_mutex locked for value copy
        m_data = other.m_data;  // copy must operate in Construction Function body, 
                                // make sure it happen after get other.m_mutex lock
    }

    threadsafe_stack& operator=( const threadsafe_stack& ) = delete;

    void push( T new_value )
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        m_data.push( std::move( new_value ) );
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        
        if( m_data.empty()) { throw empty_stack(); }
        std::shared_ptr<T> const res( std::make_shared<T>( m_data.top()));  // Once make top-value shared,
        m_data.pop();                                                       // pop() won't free the memory

        return res;

    }

    void pop( T& value )
    {
        std::lock_guard<std::mutex> lock( m_mutex );

        if( m_data.empty() ) { throw empty_stack(); }
        value = m_data.top();
        m_data.pop();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        return m_data.empty();  // this state is not reliable because after this function returned,
                                // m_mutex will be unlocked, m_data can be changed by any other thread;
    }
};
