#ifndef __THREAD_HELPER_H__
#define __THREAD_HELPER_H__

#include <unistd.h>
#include <cstdlib>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include "ffmpeg_define.h"
/**
 * Thread producer and consumer, not finish yet
 */
// https://www.cnblogs.com/haippy/p/3252092.html
// https://stackoverflow.com/questions/11711920/how-to-implement-multithread-safe-singleton-in-c11-without-using-mutex
struct ItemRepository {
    int item_buffer[THREAD_PRODUCER_SIZE];
    size_t read_position;
    size_t write_position;
    size_t item_counter;
    std::mutex mtx;
    std::mutex item_counter_mtx;
    std::condition_variable repo_not_full;
    std::condition_variable repo_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

class ThreadSingleton {
 public:
  static ThreadSingleton& Instance() {
    // Since it's a static variable, if the class has already been created,
    // it won't be created again.
    // And it **is** thread-safe in C++11.
    static ThreadSingleton myInstance;

    // Return a reference to our instance.
    return myInstance;
  }

  // delete copy and move constructors and assign operators
  ThreadSingleton(ThreadSingleton const&) = delete;             // Copy construct
  ThreadSingleton(ThreadSingleton&&) = delete;                  // Move construct
  ThreadSingleton& operator=(ThreadSingleton const&) = delete;  // Copy assign
  ThreadSingleton& operator=(ThreadSingleton &&) = delete;      // Move assign

  // Any other public methods.

 protected:
  ThreadSingleton() {
    // Constructor code goes here.
  }

  ~ThreadSingleton() {
    // Destructor code goes here.
  }

 // And any other protected methods.
}

#endif // __THREAD_HELPER_H__