#include <pthread.h>
#include <unistd.h>
#include <functional>
#include <string>
#include "Mutex.hpp"
// struct context
// {
//   thread* _this;
// };
// class thread
// {

// };
int main()
{
  pthread_mutex_t mutex_value = PTHREAD_MUTEX_INITIALIZER;
  wan::mutex wrapped_mutex(&mutex_value);
  wan::lock_guard<wan::mutex> lock(wrapped_mutex);
  return 0;
}