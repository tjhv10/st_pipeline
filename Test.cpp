#include "TaskQueue.hpp"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <thread>
#include <chrono>
#include "AO.hpp"


using namespace std;
using namespace ariel;


void func(TaskQueue& taskQueue)
{
    while (true)
    {
        void *task = taskQueue.dequeueTask();
        if (task == nullptr)
        {
            break;
        }
        int *numPtr = static_cast<int *>(task);
        cout << "dequeue: " << *numPtr << endl;
    }
}

void printNumber(void *number)
{
    unsigned int *numPtr = static_cast<unsigned int *>(number);
    cout << *numPtr << endl;
}


int main()
{
    // TaskQueue Test
    /*
    TaskQueue taskQueue;
    cout << "TaskQueue Test" << endl;
    // thread that loops and uses dequeue
    std::thread t1 = std::thread(func, std::ref(taskQueue));
    int num = 1;
    cout << "enqueue: " << num << endl;
    taskQueue.enqueueTask(&num);
    t1.join();
    */
    
    // ActiveObject Test
    ActiveObject ao;
    ao.createActiveObject(printNumber);
    unsigned int num = 1;
    ao.getQueue().enqueueTask(&num);
    this_thread::sleep_for(chrono::milliseconds(1));
    ao.stop();
    return 0;
}
