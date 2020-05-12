
#include <cstdint>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include <wiringPi.h>

#ifndef _motor_sensor_hpp
#define _motor_sensor_hpp

class motor_sensor_error : public std::runtime_error {
public:
  motor_sensor_error(const char *_message)
    : runtime_error(_message) {}
};

class motor_event {
public:
  motor_event(
           uint16_t sensor_1,
           uint16_t sensor_2,
           uint16_t value_1,
           uint16_t value_2,
           uint64_t cnt) :
           sensor_1(sensor_1),
           sensor_2(sensor_2),
           value_1(value_1),
           value_2(value_2),
           cnt(cnt) {}

  uint16_t get_id(){
    return get_sensor_1() * 100 + get_sensor_2();
  }
  uint16_t get_sensor_1(){
    return sensor_1;
  }
  uint16_t get_sensor_2(){
    return sensor_2;
  }
  uint16_t get_value_1(){
    return value_1;
  }
  uint16_t get_value_2(){
    return value_2;
  }
  uint16_t get_count(){
    return cnt;
  }
private:
  uint16_t sensor_1;
  uint16_t sensor_2;
  uint16_t value_1;
  uint16_t value_2;
  uint64_t cnt;
};

/*
class motor_sensor_worker {
public:
  motor_sensor_worker(
         uint16_t sensor_1,
         uint16_t sensor_2
         ):
         sensor_1(sensor_1),
         sensor_2(sensor_2) {
  }
  ~motor_sensor_worker(){
  }

  void start();
  static void* execute_launcher(void* args);

private:
  uint16_t sensor_1;
  uint16_t sensor_2;
  volatile bool stop = false;
  std::vector<motor_sensor_observer*> observers;

  pthread_t thread_handler;
  pthread_mutex_t mutex;
};
*/

class motor_observer {
public:
  virtual ~motor_observer() = default;
  virtual void update(motor_event&) = 0;
};

class motor_driver {
public:
  motor_driver(
         uint16_t id,
         uint16_t motor_1,
         uint16_t motor_2,
         uint16_t sensor_1,
         uint16_t sensor_2
  ) : id(id),
         motor_1(motor_1),
         motor_2(motor_2),
         sensor_1(sensor_1),
         sensor_2(sensor_2) {
  }
  ~motor_driver(){
    stop_monitor = false;
    int ret = pthread_cancel(this->thread_handler);
    if(ret != 0){
      std::cout << "motor_driver::start()... cancel:" << ret << std::endl;
    }

    observers.clear();
  }

  void init_mode();

  void stop();
  void forward();
  void backward();

  void add(motor_observer& o);
  void remove(motor_observer& o);

  void start_monitor();
  static void *executor(void* args);
  void run();

private:
  uint16_t id;
  uint16_t motor_1;
  uint16_t motor_2;
  uint16_t sensor_1;
  uint16_t sensor_2;

  std::vector<motor_observer*> observers;
  pthread_t thread_handler;
  pthread_mutex_t mutex;
  volatile uint64_t cnt = 0;
  volatile bool stop_monitor = false;
};

#endif
