
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <ctime>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <math.h>
#include <exception>
#include <vector>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#ifndef _worker_hpp
#define _worker_hpp


class worker_error : public std::runtime_error{
public:
  worker_error(const char *_message)
      : runtime_error(_message) {}
};

class worker{
public:
  virtual void run() = 0;
  void start(){
    std::cout << "worker::start start" << std::endl;
    int ret = pthread_create(
      &(this->thread_handler),
      NULL,
      worker::executor,
      this
    );
    pthread_detach(this->thread_handler);
    std::cout << "worker::start end" << std::endl;
  }
  static void *executor(void* args){
    std::cout << "worker::executor()... start"
      << std::endl;
    reinterpret_cast<worker*>(args)->run(args);
    std::cout << "worker::executor()... end"
      << std::endl;
    return nullptr;
  }
  bool get_stop(){
    return stop;
  }
  void finalize(){
    std::cout << "worker::finalize()... start"
      << std::endl;
    pthread_mutex_lock(&(this->mutex));
    std::cout << "worker::finalize()... locked"
      << std::endl;
    finalized = true;
    pthread_cond_signal(&(this->cv));
    std::cout << "worker::finalize()... after send signal"
      << std::endl;
    pthread_mutex_unlock(&(this->mutex));
    std::cout << "worker::finalize()... after unlock"
      << std::endl;
  }
  worker(){
    pthread_mutex_init(&(this->mutex), NULL);
    pthread_cond_init(&(this->cv), NULL);
  }
  ~worker(){
    std::cout << "worker::~worker()... start"
      << std::endl;
    pthread_mutex_lock(&(this->mutex));
    std::cout << "worker::~worker()... locked"
      << std::endl;
    if(!stop){
      std::cout << "worker::~worker()... set stop true"
        << std::endl;
      stop = true;
      while(!finalized){
        std::cout << "worker::~worker()... start wait cv " 
          << "[stop:" << stop << "]"
          << "[finalized:" << finalized << "]"
          << std::endl;
        pthread_cond_wait(&(this->cv), &(this->mutex));
        std::cout << "worker::~worker()... after wait cv"
          << std::endl;
      }
      std::cout << "worker::~worker()... after while"
        << std::endl;
    }
    pthread_mutex_unlock(&(this->mutex));
    std::cout << "worker::~worker()... after unlock"
      << std::endl;
  }

private:
  pthread_t thread_handler;
  pthread_mutex_t mutex;
  pthread_cond_t cv;

  volatile bool stop = false;
  volatile bool finalized = false;

  void run(void* args){
    std::cout << "worker::run(void *args)... start run"
      << std::endl;
    reinterpret_cast<worker*>(args)->run();
    std::cout << "worker::run(void *args)... end run"
      << std::endl;
    finalize();
    std::cout << "worker::run(void *args)... after finalize"
      << std::endl;
  }
};

#endif
