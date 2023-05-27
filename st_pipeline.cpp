#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include "TaskQueue.hpp"
#include "AO.hpp"

using namespace std;
using namespace ariel;

bool isPrime(unsigned int num)
{
    if (num < 2)
    {
        return false;
    }
    if (num == 2)
    {
        return true;
    }
    if (num % 2 == 0)
    {
        return false;
    }
    for (unsigned int i = 3; (i * i) <= num; i += 2)
    {
        if (num % i == 0)
        {
            return false;
        }
    }
    return true;
}

void printNumber(void *number)
{
    unsigned int *numPtr = static_cast<unsigned int *>(number);
    unsigned int num = *numPtr;
    cout << num << endl;
}

void checkPrimeAndPass(void *number, ActiveObject &ao)
{
    unsigned int *numPtr = static_cast<unsigned int *>(number);
    unsigned int num = *numPtr;
    bool isPrimeNumber = isPrime(num);
    cout << (isPrimeNumber ? "true" : "false") << endl;
    num += 11;
    *numPtr = num;
    ao.getQueue().enqueueTask(number);
}

void subtractAndPass(void *number, ActiveObject &ao)
{
    unsigned int *numPtr = static_cast<unsigned int *>(number);
    unsigned int num = *numPtr;
    bool isPrimeNumber = isPrime(num);
    cout << (isPrimeNumber ? "true" : "false") << endl;
    num -= 13;
    *numPtr = num;
    ao.getQueue().enqueueTask(number);
}

void addAndPrint(void *number)
{
    unsigned int *numPtr = static_cast<unsigned int *>(number);
    unsigned int num = *numPtr;
    num += 2;
    *numPtr = num;
    printNumber(number);
    delete numPtr;
}

int main(int argc, char *argv[])
{
    unsigned int N = 0;
    unsigned int seed = 0;

    if (argc < 2 || argc > 3)
    {
        cout << "Usage: <N> [<seed>]" << endl;
        return 1;
    }

    N = atoi(argv[1]);

    if (argc == 3)
    {
        seed = atoi(argv[2]);
    }
    else
    {
        seed = static_cast<unsigned int>(time(nullptr));
    }
    srand(seed);

    ActiveObject activeObject1;
    ActiveObject activeObject2;
    ActiveObject activeObject3;
    ActiveObject activeObject4;
    

    activeObject1.createActiveObject([&](void *number)
                                     {
        cout<<"thread 1" <<endl;
        activeObject2.getQueue().enqueueTask(number); 
        this_thread::sleep_for(chrono::milliseconds(1));});

    activeObject2.createActiveObject([&](void *number)
                                     {
        cout<<"thread 2" <<endl;
        printNumber(number);
        checkPrimeAndPass(number, activeObject3); });

    activeObject3.createActiveObject([&](void *number)
                                     {
        cout<<"thread 3" <<endl;
        printNumber(number);
        subtractAndPass(number, activeObject4); });

    activeObject4.createActiveObject([](void *number)
                                     {
        cout<<"thread 4" <<endl;
        printNumber(number);
        addAndPrint(number); });

    for (unsigned int i = 0; i < N; ++i)
    {
        unsigned int *number = new unsigned int(rand() % 900000 + 100000);
        activeObject1.getQueue().enqueueTask(number);
    }

    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "should be done\n";
    activeObject1.stop();
    activeObject2.stop();
    activeObject3.stop();
    activeObject4.stop();

    return 0;
}