#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

std::mutex mtx;
std::condition_variable cv;
std::queue<void*> taskQueue;
//bool one=false, two = false ,three = false, four =false;

void enqueueTask(void* task) {
    std::lock_guard<std::mutex> lock(mtx);
    taskQueue.push(task);
    cv.notify_one();
}

void* dequeueTask() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return !taskQueue.empty(); });

    void* task = taskQueue.front();
    taskQueue.pop();
    return task;
}

class ActiveObject {
public:
    ActiveObject() : stopFlag(false) {}

    void createActiveObject(std::function<void(void*)> func) {
        thread = std::thread(&ActiveObject::activeLoop, this, func);
    }

    void* getQueue() {
        return static_cast<void*>(&taskQueue);
    }

    void stop() {
        stopFlag = true;
        cv.notify_one();

        thread.join();
        clearQueue();
    }

private:
    void activeLoop(std::function<void(void*)> func) {
        while (true) {
            void* task = dequeueTask();
            if (stopFlag && task == nullptr) {
                break;
            }
            func(task);
        }
    }

    void clearQueue() {
        while (!taskQueue.empty()) {
            //delete taskQueue.front();
            taskQueue.pop();
        }
    }

    std::thread thread;
    bool stopFlag;
};

bool isPrime(unsigned int num) {
    if (num < 2) {
        return false;
    }
    if (num == 2) {
        return true;
    }
    if (num % 2 == 0) {
        return false;
    }
    for (unsigned int i = 3; (i * i) <= num; i += 2) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

void printNumber(void* number) {
    unsigned int* numPtr = static_cast<unsigned int*>(number);
    unsigned int num = *numPtr;
    std::cout << num << std::endl;
}

void checkPrimeAndPass(void* number) {
    unsigned int* numPtr = static_cast<unsigned int*>(number);
    unsigned int num = *numPtr;
    bool isPrimeNumber = isPrime(num);
    std::cout << (isPrimeNumber ? "true" : "false") << std::endl;
    num += 11;
    *numPtr = num;
    enqueueTask(number);
}

void subtractAndPass(void* number) {
    unsigned int* numPtr = static_cast<unsigned int*>(number);
    unsigned int num = *numPtr;
    bool isPrimeNumber = isPrime(num);
    std::cout << (isPrimeNumber ? "true" : "false") << std::endl;
    num -= 13;
    *numPtr = num;
    enqueueTask(number);
}

void addAndPrint(void* number) {
    unsigned int* numPtr = static_cast<unsigned int*>(number);
    unsigned int num = *numPtr;
    num += 2;
    *numPtr = num;
    printNumber(number);
    delete numPtr;
}

int main(int argc, char* argv[]) {
    unsigned int N = 0;
    unsigned int seed = 0;

    if (argc > 1) {
        N = std::atoi(argv[1]);
    }
    if (argc > 2) {
        seed = std::atoi(argv[2]);
    }
    else {
        seed = static_cast<unsigned int>(std::time(nullptr));
    }
    std::srand(seed);

    ActiveObject activeObject1;
    activeObject1.createActiveObject([](void* number) {
        std::cout<<"thread 1:\n";
        enqueueTask(number);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Delay before starting next AO

    ActiveObject activeObject2;
    activeObject2.createActiveObject([](void* number) {
        std::cout<<"thread 2:\n";
        printNumber(number);
        checkPrimeAndPass(number);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Delay before starting next AO

    ActiveObject activeObject3;
    activeObject3.createActiveObject([](void* number) {
        std::cout<<"thread 3:\n";
        printNumber(number);
        subtractAndPass(number);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Delay before starting next AO

    ActiveObject activeObject4;
    activeObject4.createActiveObject([](void* number) {
        std::cout<<"thread 4:\n";
        printNumber(number);
        addAndPrint(number);
    });

    for (unsigned int i = 0; i < N; ++i) {
        unsigned int* number = new unsigned int(std::rand() % 900000 + 100000);
        enqueueTask(number);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // one = false;
        // two = false;
        // three = false;
        // four =false;
    }
    std::cout<<"should be done\n";
    activeObject1.stop();
    activeObject2.stop();
    activeObject3.stop();
    activeObject4.stop();

    return 0;
}