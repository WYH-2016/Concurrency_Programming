// C++STL support std::conditon_vatiable and std::condition_varable_any 
// included in <condition_variable>
// std::condition_variable must be used with std::mutex
// std::condition_variable_any used with any type mutes BUT need more resources

#include <mutex>
#include <condition_variable>

class DataChunk {};
std::mutex m;
std::queue<DataChunk> data_queue;
std::condition_variable data_cond;

// thread A run this function to PUSH data into data_queue
void data_preparation_thread()
{
    while ( more_data_to_prepare() )
    {
        DataChunk const data = prepare_data();
        {
            std::lock_guard<std::mutex> lk( m );    // operate data_queue or use condition_variable must get mutex locked
            data_queue.push( data );                // PUSH data
        }// lock_guard be decleared in this Code Chunk, will auto destruction and unlock mutex
        data_cond.notify_one(); // std::condition_variable has member function "notify_one()" used to
                                // wake up one thread which is wating for this condition_variable instance
    }
}

// thread B run this function to POP data out from data_queue and process it
void data_processing_thread()
{
    // process data runs until there has no data in the queue
    while( true )
    {
        std::unique_lock<std::muetx> lk( m );                   // try to get the mutex lock
        // use unique_lock here becaues it maybe unlock when used with condition_variable
        // And the mutex must be locked before using with condition_variable !!!
        data_cond.wait( lk, []{ return !data_queue.empty(); }) // condition_variable member function "await"
        // first para is the mutex compare with this condition_variable
        // second para is a callable thing, use to determine weither or not conditon is valid
        // if invalide await will unlock the first para mutex, and OS let this thread hang up for waked up
        // if valid go ahead
        // when other thread call this condition_variable's ontify_one or ontify_all
        // this thread will be waked up, and try to get the mutex locked again,
        // if this thread can/t get the mutex, it will sleep angin

        DataChunk data = data_queue.front();
        data_queue.pop();

        lk.unlock();
        process_data( data );

        if( is_last_data() ) { break; }
    }
}
