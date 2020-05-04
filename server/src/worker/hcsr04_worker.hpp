#include <chrono>
#include <csignal>
#include <exception>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <set>
#include <vector>

#include <ctime>
#include <time.h>
#include <iostream>
#include <locale>
#include <sys/time.h>


#include <wiringPi.h>


#ifndef _hcsr04_worker_hpp
#define _hcsr04_worker_hpp

#define TIMEOUT 1000

class hcsr04_error : public std::runtime_error{
public:
  hcsr04_error(const char *_message)
      : runtime_error(_message) {}
};

enum hcsr04_status {
  HCSR04_SUCCESS,
  HCSR04_FAIL
}; 

class hcsr04_event {
public:
  hcsr04_event(float v)
      : distance(v),
        status(hcsr04_status::HCSR04_SUCCESS)
  {
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
  }

  hcsr04_event(hcsr04_status s) : status(s) {
    distance = 0;
  }
  hcsr04_status getStatus(){return status;};
  unsigned int getTime();
  float getDistance(){return distance;}
  struct tm* getGMTime(){return gmtime(&ts.tv_sec);}
private:

  float distance;
  hcsr04_status status;
  struct timespec ts;
};

class hcsr04_observer {
public:
  virtual ~hcsr04_observer() = default;
  virtual void update(hcsr04_event&) = 0;
};

class hcsr04_worker {
public:
  hcsr04_worker(unsigned int pt, unsigned int pe) : pinEcho(pe), pinTrig(pt) {}
  ~hcsr04_worker();
  void run();
  void start();
  int64_t diff(struct timespec *t1, struct timespec *t2);
  float getDistance(){return distance;}
  static void* executeLauncher(void* args);
  void add(hcsr04_observer& o);
  void removex(hcsr04_observer& o);

private:
  volatile bool stop = false;
  volatile float distance;
  std::vector<hcsr04_observer*> observers;
  void update(float d);
  unsigned int getTime();
  void setup();
  float mesure();

  unsigned int pinEcho;
  unsigned int pinTrig;

  pthread_t thread_handler;
  pthread_mutex_t mutex;
};

#endif
