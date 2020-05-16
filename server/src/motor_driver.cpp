
#include "motor_driver.hpp"

void motor_driver::init_mode(){
  stop_cnt = 0;

  pinMode(motor_1, OUTPUT);
  pinMode(motor_2, OUTPUT);

  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, LOW);

  pinMode(sensor_1, INPUT);
  pinMode(sensor_2, INPUT);
}

void motor_driver::stop(){
  stop_cnt = 0;
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, LOW);
  std::cout << "motor_driver::stop()! "
    << " [id:" << id << "] "
    << motor_1 << ":" << motor_2 << std::endl;
}

void motor_driver::forward(){
  forward(0);
}

void motor_driver::forward(uint64_t count){
  cnt = 0;
  stop_cnt = count;
  digitalWrite(motor_1, HIGH);
  digitalWrite(motor_2, LOW);
  std::cout << "motor_driver::forward()! "
    << " [id:" << id << "] "
    << motor_1 << ":" << motor_2 << std::endl;
}

void motor_driver::backward(){
  backward(0);
}

void motor_driver::backward(uint64_t count){
  cnt = 0;
  stop_cnt = count;
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, HIGH);
  std::cout << "motor_driver::backward()! "
    << " [id:" << id << "] "
    << motor_1 << ":" << motor_2 << std::endl;
}

void motor_driver::add(motor_observer& o) {
  observers.push_back(&o);
}

void motor_driver::remove(motor_observer& o) {
  observers.erase(std::remove(observers.begin(), observers.end(), &o));
}

void motor_driver::run(){
  std::cout << "motor_driver::run()... start"
    << " [id:" << id << "] "
    << " [stop_cnt:" << stop_cnt << "] "
    << " [sensor_1:" << sensor_1 << "] "
    << " [sensor_2:" << sensor_2 << "] "
    << std::endl;
  cnt = 0;
  uint16_t p1 = digitalRead(sensor_1);
  uint16_t p2 = digitalRead(sensor_2);
  while(!stop_monitor){
    uint16_t v1 = digitalRead(sensor_1);
    uint16_t v2 = digitalRead(sensor_2);
    if(p1 != v1 || p2 != v2){
      cnt++;
      std::cout << "motor_driver::run()... "
        << " [id:" << id << "] "
        << v1 << ":" << v2 << " "
        << cnt << " " << stop_cnt
        << std::endl;
      p1 = v1;
      p2 = v2;

      motor_event e(sensor_1,sensor_2,v1,v2,cnt);
      for(auto* o : observers){
        o->update(e);
      }
      if(stop_cnt > 0 && stop_cnt <= cnt){
        std::cout << "motor_driver::run()... count up and stop:"
          << " [id:" << id << "] "
          << stop_cnt << ":" << cnt << std::endl;
        stop();
      }
    }
  }
}

void *motor_driver::executor(void* args){
  std::cout << "motor_driver::executor()... start"
    << std::endl;
  reinterpret_cast<motor_driver*>(args)->run();
  std::cout << "motor_driver::executor()... end"
    << std::endl;
}

void motor_driver::start_monitor(){
  std::cout << "motor_driver::start_monitor()..."
    << " [id:" << id << "] "
    << std::endl;
  pthread_mutex_init(&(this->mutex), NULL);
  int ret = pthread_create(
    &(this->thread_handler),
    NULL,
    motor_driver::executor,
    this
  );
  if(ret != 0){
    std::cout << "motor_driver::start_monitor()... create:"
      << " [id:" << id << "] "
      << ret << std::endl;
  }
  std::cout << "motor_driver::start_monitor()... join"
    << " [id:" << id << "] "
    << std::endl;
  ret = pthread_detach(this->thread_handler);
  if(ret != 0){
    std::cout << "motor_driver::start_monitor()... detach:"
      << " [id:" << id << "] "
      << ret << std::endl;
  }

  //pthread_join(this->thread_handler, NULL);
  std::cout << "motor_driver::start_monitor()... done"
    << " [id:" << id << "] "
    << std::endl;
}

motor_driver::STATE motor_driver::get_state(){
  uint16_t v1 = digitalRead(motor_1);
  uint16_t v2 = digitalRead(motor_2);

  if(v1 == HIGH && v2 == LOW) {
    return FORWARD;
  } else if(v1 == LOW && v2 == HIGH) {
    return BACKWARD;
  } else if(v1 == LOW && v2 == LOW) {
    return STOP;
  }
  return UNKNOWN;
}

