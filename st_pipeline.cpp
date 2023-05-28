#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <functional>
#include <tuple>
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
    tuple<int, unsigned int> tup;
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

    tup = make_tuple(N, seed);

    ActiveObject activeObject1;
    ActiveObject activeObject2;
    ActiveObject activeObject3;
    activeObject3.createActiveObject([](void* number) {
        std::cout<<"thread 3:\n";
        printNumber(number);
        checkPrimeAndPass(number, activeObject3); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Delay before starting next AO

    ActiveObject activeObject4;
    activeObject4.createActiveObject([](void* number) {
        std::cout<<"thread 4:\n";
        printNumber(number);
        addAndPrint(number); });

    activeObject1.getQueue().enqueueTask(&tup);

    this_thread::sleep_for(chrono::milliseconds(100));
    activeObject1.stop();
    activeObject2.stop();
    activeObject3.stop();
    activeObject4.stop();

    return 0;
}