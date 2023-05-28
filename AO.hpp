#ifndef AO_HPP
#define AO_HPP
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <functional>
#include "TaskQueue.hpp"

using namespace std;

namespace ariel
{
    class ActiveObject
    {
    public:
        ActiveObject() : stopFlag(false) {}

        void createActiveObject(function<void(void *)> func)
        {
            thread = std::thread(&ActiveObject::activeLoop, this, func);
        }

        TaskQueue &getQueue()
        {
            return taskQueue;
        }

        void stop()
        {
            stopFlag = true;
            thread.join();
            taskQueue.clearQueue();
        }

    private:
        std::thread thread;
        bool stopFlag;
        TaskQueue taskQueue;

        void activeLoop(function<void(void *)> func)
        {
            void *task;
            while (!stopFlag)
            {
                task = this->taskQueue.dequeueTask();
                if (task)
                {
                    func(task);
                }
            }
        }
    };
}

#endif