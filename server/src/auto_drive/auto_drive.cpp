#include <chrono>
#include <pthread.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <sys/time.h>

#include <wiringPi.h>

#include "auto_drive.hpp"
#include "../worker/vl53l0x_worker.hpp"
#include "../worker/gy271_worker.hpp"
#include "auto_drive.hpp"

void auto_drive::add(auto_drive_observer& o){
  observers.push_back(&o);
}

void auto_drive::remove(auto_drive_observer& o){
  observers.erase(std::remove(observers.begin(), observers.end(), &o));
}

void* auto_drive::execute_launcher(void* args){
  std::cout << "auto_drive::execute_launcher()... start" << std::endl;
  reinterpret_cast<auto_drive*>(args)->run();
  std::cout << "auto_drive::execute_launcher()... end" << std::endl;
}

void auto_drive::start() {
  std::cout << "auto_drive::start()..." << std::endl;
  pthread_mutex_init(&(this->mutex), NULL);
  pthread_cond_init(&(this->cv), NULL);
  pthread_create(
    &(this->thread_handler),
    NULL,
    auto_drive::execute_launcher,
    this
  );
  std::cout << "auto_drive::start()... thread created" << std::endl;
  pthread_detach(this->thread_handler);
  std::cout << "auto_drive::start()... thread detached" << std::endl;
}

void auto_drive::empty_queue(){
  pthread_mutex_lock(&(this->mutex));
  while(!action_queue.empty()){
    action_queue.pop();
  }
  pthread_cond_signal(&(this->cv));
  pthread_mutex_unlock(&(this->mutex));
}

void auto_drive::stop_action_queue(){
  action_loop_stop = false;
}

void auto_drive::stop_auto_drive(){
  pthread_mutex_lock(&(this->mutex));
  stop_main = false;
  pthread_cond_signal(&(this->cv));
  pthread_mutex_unlock(&(this->mutex));
}

void auto_drive::add(shared_ptr<drive_action> action){
  std::cout << "aut_drive::add start" << std::endl;
  pthread_mutex_lock(&(this->mutex));
  std::cout << "aut_drive::add locked" << std::endl;
  action_queue.push(action);
  std::cout << "aut_drive::add pushed" << std::endl;
  pthread_cond_signal(&(this->cv));
  std::cout << "aut_drive::add signaled" << std::endl;
  pthread_mutex_unlock(&(this->mutex));
  std::cout << "aut_drive::add unlocked" << std::endl;
}

void auto_drive::run() {
  std::cout << "auto_drive::run(): start..." << std::endl;

  if (stop_main) {
    std::cerr << "Stop Flag is true!" << std::endl;
    return;
  }

  std::cout << "auto_drive::run(): start loop" << std::endl;
  while (!stop_main) {
    std::cout << "auto_drive::run lock..." << std::endl;
    pthread_mutex_lock(&(this->mutex));
    std::cout << "auto_drive::run check queue..." << std::endl;
    while(action_queue.empty() && !stop_main){
      std::cout << "auto_drive::run wait..." << std::endl;
      pthread_cond_wait(&(this->cv), &(this->mutex));
      std::cout << "auto_drive::run wait exit" << std::endl;
    }
    std::cout << "auto_drive::run front" << std::endl;
    std::shared_ptr<drive_action> action = action_queue.front();
    std::cout << "auto_drive::run pop" << std::endl;
    action_queue.pop();
    action_loop_stop = false;
    std::cout << "auto_drive::run unlock" << std::endl;
    pthread_mutex_unlock(&(this->mutex));

    std::cout << "auto_drive::run before action loop" << std::endl;
    while(action != nullptr && !action_loop_stop && !stop_main){
      std::cout << "auto_drive::run do_action" << std::endl;
      action->do_action();
      std::cout << "auto_drive::run do_action done" << std::endl;
      action = action->get_next_action();
      std::cout << "auto_drive::run do_action next action" << std::endl;
    }
  }
}



