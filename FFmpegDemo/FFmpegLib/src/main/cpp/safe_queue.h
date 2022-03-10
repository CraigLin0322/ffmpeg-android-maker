#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include <queue>
#include <mutex>
#include <condition_variable>
//Refer to https://stackoverflow.com/questions/15278343/c11-thread-safe-queue
template <class T>
class SafeQueue {
private:
    std::queue <T> queue;
    mutable std::mutex mutex;
    std::condition_variable variable;
public:
    SafeQueue(void){

    }

    ~SafeQueue(void) {

    }

    void enqueue(T t){
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(t);
        variable.notify_one();
    }

    T dequeue(void){
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty()) {
            variable.wait(lock);
        }
        T val = queue.front();
        queue.pop();
        return val;
    }

};

#endif // __SAFE_QUEUE_H__
