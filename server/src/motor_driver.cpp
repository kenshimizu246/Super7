
#include "motor_driver.hpp"

void motor_driver::init_mode(){
  pinMode(motor_1, OUTPUT);
  pinMode(motor_2, OUTPUT);

  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, LOW);

  pinMode(sensor_1, INPUT);
  pinMode(sensor_2, INPUT);
}

void motor_driver::stop(){
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, LOW);
  std::cout << "motor_driver::stop()! " << motor_1 << ":" << motor_2 << std::endl;
}

void motor_driver::forward(){
  digitalWrite(motor_1, HIGH);
  digitalWrite(motor_2, LOW);
  std::cout << "motor_driver::forward()! " << motor_1 << ":" << motor_2 << std::endl;
}

void motor_driver::backward(){
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, HIGH);
  std::cout << "motor_driver::backward()! " << motor_1 << ":" << motor_2 << std::endl;
}

void motor_driver::add(motor_observer& o) {
  observers.push_back(&o);
}

void motor_driver::remove(motor_observer& o) {
  observers.erase(std::remove(observers.begin(), observers.end(), &o));
}

void motor_driver::run(){
  std::cout << "motor_driver::run()... start" << std::endl;
  cnt = 0;
  uint16_t p1 = digitalRead(sensor_1);
  uint16_t p2 = digitalRead(sensor_2);
  while(!stop_monitor){
    uint16_t v1 = digitalRead(sensor_1);
    uint16_t v2 = digitalRead(sensor_2);
    if(p1 != v1 || p2 != v2){
      cnt++;
      std::cout << "motor_driver::run()... "
        << v1 << ":" << v2 << " " << cnt << std::endl;
      p1 = v1;
      p2 = v2;

      motor_event e(0,0,v1,v2,cnt);
      for(auto* o : observers){
        o->update(e);
      }
    }
  }
}

void *motor_driver::executor(void* args){
  std::cout << "motor_driver::executor()... start" << std::endl;
  reinterpret_cast<motor_driver*>(args)->run();
  std::cout << "motor_driver::executor()... end" << std::endl;
}

void motor_driver::start_monitor(){
  std::cout << "motor_driver::start_monitor()..." << std::endl;
  pthread_mutex_init(&(this->mutex), NULL);
  int ret = pthread_create(
    &(this->thread_handler),
    NULL,
    motor_driver::executor,
    this
  );
  if(ret != 0){
    std::cout << "motor_driver::start_monitor()... create:" << ret << std::endl;
  }
  std::cout << "motor_driver::start_monitor()... join" << std::endl;
  ret = pthread_detach(this->thread_handler);
  if(ret != 0){
    std::cout << "motor_driver::start_monitor()... detach:" << ret << std::endl;
  }

  //pthread_join(this->thread_handler, NULL);
  std::cout << "motor_driver::start_monitor()... done" << std::endl;
}


