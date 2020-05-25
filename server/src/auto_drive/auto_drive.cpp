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

void auto_drive::update(){
  auto_drive_event e;
  for(auto* o : observers){
    o->update(e);
  }
}

void* auto_drive::execute_launcher(void* args){
  std::cout << "auto_drive::execute_launcher()... start" << std::endl;
  reinterpret_cast<auto_drive*>(args)->run();
  std::cout << "auto_drive::execute_launcher()... end" << std::endl;
}

void auto_drive::start() {
  std::cout << "auto_drive::start()..." << std::endl;
  pthread_mutex_init(&(this->mutex), NULL);
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

void auto_drive::update(vl53l0x_event& event){
  //latest_vl53l0x_event = event;
}

void auto_drive::update(gy271_event& event){
  //latest_gy271_event = event;
}

void auto_drive::run() {
  std::cout << "auto_drive::run(): start..." << std::endl;

  if (stop) {
    std::cerr << "Stop Flag is true!" << std::endl;
    return;
  }

//  std::shared_ptr<drive_action> action(new forward_action(driver));
//  drive_actions.push(action);

  std::cout << "auto_drive::run(): start loop" << std::endl;
  int cnt = 0;
  for (;!stop; ++cnt) {
    //usleep(1*1000000);
    std::cout << "before front..." << std::endl;
    std::shared_ptr<drive_action> action = drive_actions.front();
    std::cout << "after front..." << std::endl;
    //drive_actions.pop();
    //action->do_action();
  }
}



