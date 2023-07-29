// a simple thread pool
#include<atomic>
#include<thread>
#include<functional>

#include "../ch8_design_concurrency_code/3_join_threads.h"
#include "../ch6_concurrency_data_structure_based_lock/7_threadsafe_queue.hpp"

class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<std::function<void()>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while(!done)
        {
            std::function<void()> task;
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
    void submit(FunctionType f)
    {
        work_queue.push(std::function<void()>(f));
    }
};