#include <chrono>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>

using namespace std;

const int BUFFER_SIZE = 5;
int counter = 0;

mutex mtx;
condition_variable sema_empty, sema_full;

void producer()
{
    while (true)
    {
        unique_lock<mutex> lock(mtx);
        sema_empty.wait(lock, []
                        { return counter < BUFFER_SIZE; }); // Wait if counter is full

        counter++;
        cout << "Mr. Simpson baked a pizza  \n\t Total pizzas on counter: " << counter << endl;

        lock.unlock();
        sema_full.notify_one();

        this_thread::sleep_for(chrono::seconds(1));
    }
}

void consumer()
{
    while (true)
    {
        unique_lock<mutex> lock(mtx);
        sema_full.wait(lock, []
                       { return counter > 0; });

        counter--;
        cout << "Joey ate a pizza  \n\t Pizzas left on counter: " << counter << endl;

        lock.unlock();
        sema_empty.notify_one(); // Signal producer that space is available

        this_thread::sleep_for(chrono::seconds(3)); // Simulate eating
    }
}

int main()
{
    thread producerThread(producer);
    thread consumerThread(consumer);

    producerThread.join();
    consumerThread.join();

    return 0;
}