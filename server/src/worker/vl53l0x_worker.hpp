#include "../vl53l0x/VL53L0X.hpp"

#include <chrono>
#include <csignal>
#include <exception>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <set>
#include <vector>
#include <time.h>

#include <ctime>
#include <iostream>
#include <locale>


#ifndef _vl53l0x_worker_hpp
#define _vl53l0x_worker_hpp

class vl53l0x_error : public std::runtime_error{
public:
  vl53l0x_error(const char *_message)
      : runtime_error(_message) {}
};

enum vl53l0x_status {
  SUCCESS,
  FAIL
}; 

class vl53l0x_event {
public:
  vl53l0x_event(uint16_t id, uint16_t v)
      : id(id), distance(v),
        status(vl53l0x_status::SUCCESS)
  {
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
  }

  vl53l0x_event(uint16_t id, vl53l0x_status s) : id(id), status(s) {
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
    distance = 8096;
  }
  uint16_t getID(){return id;}
  uint16_t getDistance(){return distance;}
  struct timespec getTime(){return ts;}
  struct tm* getGMTime(){return gmtime(&ts.tv_sec);}
  int getStrTime(char* b){
    //strftime(b, sizeof(b), "%D %T", gmtime(&ts.tv_sec));
    return strftime(b, sizeof(b), "%D %T", gmtime(&ts.tv_sec));
  }
  vl53l0x_status getStatus(){return status;};

private:
  uint16_t id;
  uint16_t distance;
  vl53l0x_status status;
  struct timespec ts;
};

class vl53l0x_observer {
public:
  virtual ~vl53l0x_observer() = default;
  virtual void update(vl53l0x_event&) = 0;
};

class vl53l0x_worker {
public:
  vl53l0x_worker(uint16_t id) : id(id) {}
  ~vl53l0x_worker();
  void init(){};
  void run();
  void start();
  void setHighSpeed(bool b);
  void setHighAccuracy(bool b);
  void setLongRange(bool b);
  uint16_t getDistance();
  static void* executeLauncher(void* args);
  void add(vl53l0x_observer& o);
  void removex(vl53l0x_observer& o);
  uint16_t getID();

  const static uint64_t DISTANCE_ERROR = 8096;

private:
  uint16_t id = 0;

  bool high_speed = false;
  bool high_accuracy = false;
  bool long_range = false;
  volatile bool stop = false;
  volatile uint16_t distance = DISTANCE_ERROR;
  std::vector<vl53l0x_observer*> observers;
  void update(uint16_t d);

  uint64_t totalDuration = 0;
  uint64_t maxDuration = 0;
  uint64_t minDuration = 1000*1000*1000;

  pthread_t thread_handler;
  pthread_mutex_t mutex;
};

#endif
