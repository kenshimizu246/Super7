#include <chrono>
#include <pthread.h>
#include <memory>
#include <vector>
#include <algorithm>

#include <sys/time.h>

#include <wiringPi.h>

#include "hcsr04_worker.hpp"

void hcsr04_worker::setup(){
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
  digitalWrite(pinTrig, LOW);
}

void hcsr04_worker::add(hcsr04_observer& o){
  observers.push_back(&o);
}

void hcsr04_worker::removex(hcsr04_observer& o){
  observers.erase(std::remove(observers.begin(), observers.end(), &o));
}

void hcsr04_worker::update(float d){
  hcsr04_event e(d);
  for(auto* o : observers){
    o->update(e);
  }
}

void* hcsr04_worker::executeLauncher(void* args){
  std::cout << "hcsr04_worker::executeLauncher()... start" << std::endl;
  reinterpret_cast<hcsr04_worker*>(args)->run();
  std::cout << "hcsr04_worker::executeLauncher()... end" << std::endl;
}

void hcsr04_worker::start() {
  std::cout << "hcsr04_worker::start()..." << std::endl;
  pthread_mutex_init(&(this->mutex), NULL);
  pthread_create(
    &(this->thread_handler),
    NULL,
    hcsr04_worker::executeLauncher,
    this
  );
  std::cout << "hcsr04_worker::start()... thread created" << std::endl;
  pthread_detach(this->thread_handler);
  std::cout << "hcsr04_worker::start()... thread detached" << std::endl;
}

int64_t hcsr04_worker::diff(struct timespec *t1, struct timespec *t2) {
  return ((t1->tv_sec - t2->tv_sec) * 1000000000) + (t1->tv_nsec - t2->tv_nsec);
}

float hcsr04_worker::mesure() {
  struct timespec prev, signoff, signon;

  digitalWrite(pinTrig, HIGH);
  usleep(0.00001 * 1000000); // microsecond... 1sec = 1 * 1000000
  digitalWrite(pinTrig, LOW);
  clock_gettime(CLOCK_MONOTONIC, &prev);
  while(digitalRead(pinEcho) == LOW){
    clock_gettime(CLOCK_MONOTONIC, &signoff);
    if(diff(&prev, &signoff) > TIMEOUT){
      printf("prev: %ld, %ld, signoff: %ld, %ld \n", prev.tv_sec, prev.tv_nsec, signoff.tv_sec, signoff.tv_nsec);
      return -1;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &prev);
  while(digitalRead(pinEcho) == HIGH){
    clock_gettime(CLOCK_MONOTONIC, &signon);
    if(diff(&prev, &signoff) > TIMEOUT){
      printf("prev: %ld, %ld, signon: %ld, %ld \n", prev.tv_sec, prev.tv_nsec, signon.tv_sec, signon.tv_nsec);
      return -1;
    }
  }
  printf("signdiff: %lld\n", diff(&signon, &signoff));
  return (float)((diff(&signon, &signoff) * 34)/2/1000);
}


void hcsr04_worker::run() {
  std::cout << "hcsr04_worker::run(): start..." << std::endl;

  setup();

  if (stop) {
    std::cerr << "Stop Flag is true!" << std::endl;
    return;
  }

  std::cout << "hcsr04_worker::run(): start loop" << std::endl;
  int cnt = 0;
  for (;!stop; ++cnt) {
    distance =  mesure();
    printf("distance:%f\n", distance);
    update(distance);
    usleep(1*1000000);
  }
}

hcsr04_worker::~hcsr04_worker(){

}


