#ifndef TASKQUEUE_H
#define TASKQUEUE_H
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

namespace ariel
{
    class TaskQueue
    {
    private:
        queue<void *> taskQueue;
        mutex mtx;
        condition_variable cv;

    public:
        void enqueueTask(void *task)
        {
            unique_lock<mutex> lock(mtx);
            taskQueue.push(task);
            lock.unlock();
            cv.notify_one();
        }

        void *dequeueTask()
        {
            unique_lock<mutex> lock(mtx);
            if (cv.wait_for(lock, chrono::milliseconds(100), [this]
                            { return !taskQueue.empty(); }) == false)
            {
                return nullptr;
            }
            void *task = taskQueue.front();
            taskQueue.pop();
            return task;
        }

        queue<void *> &getQueue()
        {
            return taskQueue;
        }

        void clearQueue()
        {
            while (!taskQueue.empty())
            {
                taskQueue.pop();
            }
        }
    };
}

#endif