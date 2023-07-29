// When a thread want to get more than one mutex, it maybe get DEAD_LOCK
// To avoid DEAD_LOCK, there are something useful methods, like Always get mutexs locked in same order
// it may be effective, but conside following situation

class BigObj;

void swap(BigObj& lhs, BigObj& rhs);

class X
{
private:
    BigObj m_detial;
    std::mutex m_mutex;
public;
    X(BigObj const& sd) : m_detail(sd) {}
    
    // this swap looks like get mutexs locked in same order
    // but when the two parameters switch orders
    // the mutexs lock order will be changed!
    friend void swap(X& lhs, X&rhs)
    {
        if(lhs == rhs) return;

        std::lock_guard<std::mutex> lock_a( lhs.m_mutex );
        std::lock_guard<std::mutex> lock_b( rhs.m_mutex );
        swap(lhs.m_detials, rhs.m_details);
    }
    
    // To deal with the upper situation
    // We must lock more then one mutexs at once
    // And "std::lock" can get more than one parameters, C++ will make sure NO_DEAD_LOCK
    friend void swap(X& lhs, X&rhs)
    {
        if(lhs == rhs) return;
        
        std::lock(lhs.m_mutex, rhs.m_mutex);    // get two mutex locked at once
        std::lock_guard<std::mutex> lock_a( lhs.m_mutex, std::adopt_lock );
        std::lock_guard<std::mutex> lock_b( rhs.m_mutex, std::adopt_lock ); // std::adopt_lock means lock_guard get a mutex has been locked
        swap(lhs.m_detials, rhs.m_details);
    }
    
    // C++17 gives a new method: scope_lock, which totaly equal to lock_guard
    // But cope_lock is "vatiable template", this means it can get different type parameters
    friend void swap(X& lhs, X&rhs)
    {
        if(lhs == rhs) return;

        std::scope_lock<std::mutex, std::mutex> guard( lhs.m_mutex, rhs.m_mutex);
        swap(lhs.m_detials, rhs.m_details);
    }

};

