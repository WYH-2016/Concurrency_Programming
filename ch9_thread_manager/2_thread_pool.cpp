// File 1_thread_pool.cpp design a simple thread pool, call submit to add tasks, threads run tasks automatic
// But it's too simple to get return from tasks, so we design 2_thread_pool

#include<atomic>
#include<thread>
#include<functional>
#include<memory>
#include<future>

#include "../ch8_design_concurrency_code/3_join_threads.h"
#include "../ch6_concurrency_data_structure_based_lock/7_threadsafe_queue.hpp"

// functionj_wapper can wapper Any-Type callable object to same object to save in a container
class function_wrapper
{
    struct impl_base{
        virtual void call() = 0;
        virtual ~impl_base() {}
    };

    std::unique_ptr<impl_base> impl;

    // drive impl_base to get different type function
    template<typename FunctionType>
    struct impl_type: impl_base
    {
        FunctionType f;
        impl_type(FunctionType&& _f):
            f(std::move(_f))
        {}

        void call() { f(); }
    };

public:
    template< typename F>
    function_wrapper(F&& f):
        impl(new impl_type<F>(std::move(f)))
    {}

    void operator()() { impl->call(); }
    function_wrapper() = default;
    function_wrapper(function_wrapper&& other):
        impl(std::move(other.impl))
    {}

    function_wrapper& operator=(function_wrapper&& other)
    {
        impl=std::move(other.impl);
        return *this;
    }
    function_wrapper(const function_wrapper&)=delete;
    function_wrapper(function_wrapper&) = delete;
    function_wrapper& operator=(const function_wrapper&)=delete;
};

class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while(!done)
        {
            function_wrapper task;
            if(work_queue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }
public:
    thread_pool():
        done(false), joiner(threads)
    {
        unsigned const thread_count = std::thread::hardware_concurrency();

        try
        {
            for(unsigned i = 0; i < thread_count; ++i)
            {
                threads.push_back(std::thread(&thread_pool::worker_thread, this));
            }
        }
        catch(...)
        {
            done = true;
            throw;
        }
        
    }

    ~thread_pool()
    {
        done = true;
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> 
        submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));   // function_warrper is a unique_ptr<impl_base>
        return res;
    }
};