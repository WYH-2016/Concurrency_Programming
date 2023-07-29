// std::packaged_task is a class template
// template para is a function signature
// when define a std::packaged_task instance
// it has a std::future in and bind to the argument function
// you can use task() make the function runs asynchoronizely
// you can call get_future to get it, and call get() or wait() on thr std::future as usual

#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex m;
std::deque<std::packaged_task<void()>> tasks;
bool gui_shutdown_message_received();
void get_and_process_gui_message();

void gui_thread()
{
    while( !gui_shutdown_message_receivesd() )
    {
        get_and_process_gui_message();

        std::packaged_task<void()> task;            // define a empty task to get task from task_deque
        {
            std::lock_guard<std::mutex> lk( m );    // lock the task_deque
            if( tasks.empty() )
                continue;
            task = std::move( task.front(); );      // get a task
            tasks.pop_froint();
        }
        task();                                     // let it run
    }
}

std::thread gui_bg_thread( gui_thread );

template<typename Func>
std::future<void> post_task_for_gui_thread( Func f )
{
    std::packaged_task<void()> task( f );       // define task by para callable thing ( Func f )
    std::future<void> res = task.get_future();  // get the std::future install bind to this task
    std::lockl_guard<std::mutex> lk( m )        // lock the task_deque mutex
    tasks.push_back( std::move( task ) );       // push the task into deque ( task instance is move only)
    return res;                                 // return thr std::future instance for use
}
