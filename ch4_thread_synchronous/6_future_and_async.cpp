// condition_variable used when some condition changes timely
// But if you just want wait for something happen oncely
// std::future and std::async() is much more suitable

#include <future>
#include <iostream>

int find_the_answer();

void do_some_other_thing();

int main()
{
    std::future<int> the_answer = std::async( find_the_answer );
    do_some_other_thing();
    std::cout << "The Answer is " << the_answer.get() << std::endl;
}

// std::future is a Template, it's instance can store a value
// The value's type is the template parameter
// std::async() get a callable thing, and run it in different ways
//  - std::launch::deffered means run in same thread when call the std::future get() or wait()
//  - std::launch::async means run in new thread
