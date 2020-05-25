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
#include <queue>
#include <sys/time.h>


#include <wiringPi.h>

#include "../config.hpp"
#include "../worker/vl53l0x_worker.hpp"
#include "../worker/gy271_worker.hpp"
#include "../pca9685.hpp"
#include "../mecanum_driver.hpp"

using namespace tamageta;

#ifndef _auto_drive_hpp
#define _auto_drive_hpp

class auto_drive_error : public std::runtime_error{
public:
  auto_drive_error(const char *_message)
      : runtime_error(_message) {}
};

class auto_drive_event {
public:
  enum auto_drive_status {
    SUCCESS,
    FAIL
  }; 

  auto_drive_event():
        status(SUCCESS)
  {
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
  }

  auto_drive_event(auto_drive_status s) : status(s) {
  }
  auto_drive_status get_status(){return status;};
  struct tm* get_gmtime(){return gmtime(&ts.tv_sec);}
private:

  auto_drive_status status;
  struct timespec ts;
};

class auto_drive_observer {
public:
  virtual ~auto_drive_observer() = default;
  virtual void update(auto_drive_event&) = 0;
};

class drive_action : public vl53l0x_observer, public gy271_observer {
public:
  mecanum_driver& driver;
  pca9685& servo;
  vl53l0x_worker& vl53l0x;
  gy271_worker& gy271;

  drive_action(mecanum_driver& m, pca9685& s, vl53l0x_worker& v, gy271_worker& g) : driver(m), servo(s), vl53l0x(v), gy271(g) {
    vl53l0x.add(*this);
    gy271.add(*this);
  }
  virtual ~drive_action(){
    vl53l0x.remove(*this);
    gy271.remove(*this);
  }
  virtual void do_action() = 0;
  virtual void set_next_action(shared_ptr<drive_action> action) = 0;
  virtual shared_ptr<drive_action> get_next_action() = 0;

  void update(vl53l0x_event& event){
  }
  void update(gy271_event& event){
  }
  void start(){
    pthread_mutex_init(&(this->mutex), NULL);
    int ret = pthread_create(
      &(this->thread_handler),
      NULL,
      drive_action::executor,
      this
    );
    pthread_join(this->thread_handler, NULL);
  }

  pthread_t thread_handler;
  pthread_mutex_t mutex;
  pthread_cond_t cv;

private:
  static void *executor(void* args){
    std::cout << "drive_action::executor()... start"
      << std::endl;
    reinterpret_cast<drive_action*>(args)->do_action();
    std::cout << "drive_action::executor()... end"
      << std::endl;
  }

};

class stop_action : public drive_action {
public:
  void do_action(){
    driver.stop();
  }
  void set_next_action(shared_ptr<drive_action> action){}
  shared_ptr<drive_action> get_next_action(){
    return nullptr;
  }
};

class move_forward_action : public drive_action {
public:
  move_forward_action(mecanum_driver& m, pca9685& s, 
  vl53l0x_worker& v, gy271_worker& g,
              mecanum_driver::STATE st, uint16_t d)
              : drive_action(m, s, v, g), state(st), distance(d) {}
  void update(vl53l0x_event& event){
    pthread_mutex_lock(&(this->mutex));
    if(distance > event.get_distance() && !stop){
      stop = true;
      pthread_cond_signal(&(this->cv));
    }
    pthread_mutex_unlock(&(this->mutex));
  }
  void update(gy271_event& event){
  }
  void set_state(mecanum_driver::STATE s){
    state = s;
  }
  void do_action(){
    if(mecanum_driver::STATE::FORWARD == state){
      driver.forward();
    } else if(mecanum_driver::STATE::FORWARD_RIGHT == state){
      driver.forward_right();
    } else if(mecanum_driver::STATE::FORWARD_LEFT == state){
      driver.forward_left();
    } else if(mecanum_driver::STATE::SLIDE_RIGHT == state){
      driver.slide_right();
    } else if(mecanum_driver::STATE::SLIDE_LEFT == state){
      driver.slide_left();
    } else {
      throw auto_drive_error("unsupported state!");
    }

    pthread_mutex_lock(&(this->mutex));
    while(!stop){
      pthread_cond_wait(&(this->cv),&(this->mutex));
    }
    pthread_mutex_unlock(&(this->mutex));
  }
  shared_ptr<drive_action> get_next_action(){
    return next_action;
  }
  void set_next_action(shared_ptr<drive_action> n){
    next_action = n;
  }
  
private:
  uint16_t distance;
  bool stop = false;
  mecanum_driver::STATE state;
  shared_ptr<drive_action> next_action;
};

class round_action : public drive_action {
public:
  void update(vl53l0x_event& event){
    pthread_mutex_lock(&(this->mutex));
    if(distance < event.get_distance() && !stop){
      stop = true;
      pthread_cond_signal(&(this->cv));
    }
    pthread_mutex_unlock(&(this->mutex));
  }
  void update(gy271_event& event){
  }
  void do_action(){
    driver.round_right();
    pthread_mutex_lock(&(this->mutex));
    while(!stop){
      pthread_cond_wait(&(this->cv),&(this->mutex));
    }
    pthread_mutex_unlock(&(this->mutex));
  }
  shared_ptr<drive_action> get_next_action(){
    return nullptr;
  }
  void set_next_action(shared_ptr<drive_action> action){
    next_action = action;
  }
  
private:
  uint16_t distance = 100;
  bool stop = false;
  shared_ptr<drive_action> next_action;
};

class check_and_forward_action : public drive_action {
public:
  check_and_forward_action(mecanum_driver& m, pca9685& s, vl53l0x_worker& v, gy271_worker& g) : drive_action(m, s, v, g) {}
  void update(vl53l0x_event& event){
    pthread_mutex_lock(&(this->mutex));
    if(!read){
      distance = event.get_distance();
      read = true;
      pthread_cond_signal(&(this->cv));
    }
    pthread_mutex_unlock(&(this->mutex));
  }
  void update(gy271_event& event){
  }
  void do_action(){
    uint16_t max = config::get_instance().get_front_vl53l0x_servo_max();
    uint16_t min = config::get_instance().get_front_vl53l0x_servo_min();
    uint16_t id = config::get_instance().get_front_vl53l0x_servo_id();

    uint16_t mm = 0;
    state = mecanum_driver::STATE::STOP;

    // SLIDE_RIGHT
    servo.PwmWrite(id, 0, min);
    pthread_mutex_lock(&(this->mutex));
    read = false;
    while(!read){
      pthread_cond_wait(&(this->cv),&(this->mutex));
    }
    if(mm < distance){
      mm = distance;
      state = mecanum_driver::STATE::SLIDE_RIGHT;
    }
    pthread_mutex_unlock(&(this->mutex));

    // FORWARD_RIGHT
    servo.PwmWrite(id, 0, ((max - min) * 0.25 + min));
    pthread_mutex_lock(&(this->mutex));
    read = false;
    while(!read){
      pthread_cond_wait(&(this->cv),&(this->mutex));
    }
    if(mm < distance){
      mm = distance;
      state = mecanum_driver::STATE::FORWARD_RIGHT;
    }
    pthread_mutex_unlock(&(this->mutex));

    // FORWARD
    servo.PwmWrite(id, 0, ((max - min) * 0.5 + min));
    pthread_mutex_lock(&(this->mutex));
    read = false;
    while(!read){
      pthread_cond_wait(&(this->cv),&(this->mutex));
    }
    if(mm < distance){
      mm = distance;
      state = mecanum_driver::STATE::FORWARD;
    }
    pthread_mutex_unlock(&(this->mutex));

    // FORWARD_LEFT
    servo.PwmWrite(id, 0, ((max - min) * 0.75 + min));
    pthread_mutex_lock(&(this->mutex));
    read = false;
    while(!read){
      pthread_cond_wait(&(this->cv),&(this->mutex));
    }
    if(mm < distance){
      mm = distance;
      state = mecanum_driver::STATE::FORWARD_LEFT;
    }
    pthread_mutex_unlock(&(this->mutex));

    // SLIDE_LEFT
    servo.PwmWrite(id, 0, max);
    pthread_mutex_lock(&(this->mutex));
    read = false;
    while(!read){
      pthread_cond_wait(&(this->cv),&(this->mutex));
    }
    if(mm < distance){
      mm = distance;
      state = mecanum_driver::STATE::SLIDE_LEFT;
    }
    pthread_mutex_unlock(&(this->mutex));
  }

  shared_ptr<drive_action> get_next_action(){
    if(state == mecanum_driver::STATE::STOP
        || state == mecanum_driver::STATE::UNKNOWN){
      return nullptr;
    }
    next_action->set_state(state);
    return next_action;
  }
  void set_next_action(shared_ptr<drive_action> action){
    next_action = static_pointer_cast<move_forward_action>(action);
  }
  
private:
  uint16_t distance;
  mecanum_driver::STATE state;
  bool read = true;
  shared_ptr<move_forward_action> next_action;
};

class auto_drive : public vl53l0x_observer, public gy271_observer {
public:
  auto_drive(pca9685& s, mecanum_driver& d, vl53l0x_worker& v, gy271_worker& g)
    : servo(s), driver(d), vl53l0x(v), gy271(g) {}
  ~auto_drive(){};
  void run();
  void start();
  void add(auto_drive_observer& o);
  void remove(auto_drive_observer& o);
  void update(vl53l0x_event& event);
  void update(gy271_event& event);

private:
  volatile bool stop = false;
  std::vector<auto_drive_observer*> observers;

  std::mutex action_lock;
  std::queue<std::shared_ptr<drive_action>> drive_actions;

  pca9685& servo;
  mecanum_driver& driver;
  vl53l0x_worker& vl53l0x;
  gy271_worker& gy271;

  static void* execute_launcher(void* args);
  void update();
  unsigned int get_gmtime();

  pthread_t thread_handler;
  pthread_mutex_t mutex;
  pthread_cond_t cv;
  pthread_condattr_t cattr;
};

#endif
