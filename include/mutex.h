#ifndef MUTEX__CLASS__H__
#define MUTEX__CLASS__H__

// Prototype mutex class

#ifdef _REENTRANT

#include <pthread.h>

class Mutex
{
  pthread_mutex_t mutex;
public:
  Mutex()
    {
      pthread_mutex_t tmp = PTHREAD_MUTEX_INITIALIZER;
      mutex = tmp;
      pthread_mutex_init(&mutex, 0);
    }
  ~Mutex()
    {
      pthread_mutex_destroy(&mutex);
    }
  int lock()
    {
      return pthread_mutex_lock(&mutex);
    }
  int trylock()
    {
      return pthread_mutex_trylock(&mutex);
    }
  int unlock()
    {
      return pthread_mutex_unlock(&mutex);
    }
};

#else // _REENTRANT

class Mutex
{
public:
  Mutex() { }
  ~Mutex() { }
  int lock() const { return 0; }
  int trylock() const { return 0; }
  int unlock() const { return 0; }
};

#endif // _REENTRANT

#endif
