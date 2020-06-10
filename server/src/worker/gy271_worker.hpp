
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

#include "worker.hpp"

#ifndef _gy271_worker_hpp
#define _gy271_worker_hpp

#define GY_271_ADDR 0x0d

#define REG_CTRL 0x09

// mode 00:Standby 01:Continuous
#define MODE_STANDBY    0x00
#define MODE_CONTINUOUS 0x01

// ODR (output Data Rate) 00:10Hz 01:50Hz 10:100Hz 11:200Hz
#define ODR_10HZ  0x00
#define ODR_50HZ  0x04
#define ODR_100HZ 0x08
#define ODR_200HZ 0x0C

// RNG (Full Scale) 00:2G 01:8G
#define RNG_2G 0x00
#define RNG_8G 0x10

// OSR (Over Sample Ratio) 
#define OSR_512 0x00
#define OSR_256 0x40
#define OSR_128 0x80
#define OSR_64  0xC0

class gy271_error : public std::runtime_error{
public:
  gy271_error(const char *_message)
      : runtime_error(_message) {}
};

enum gy271_status {
  GY271_SUCCESS,
  GY271_FAIL,
  GY271_NO_NEW_DATA,
  GY271_DATA_OVERFLOW,
  GY271_DATA_SKIPPED_FOR_READING,
  GY271_NOT_ENOUGH
};

class gy271_event {
public:
  gy271_event(float heading,
        int x, int y, int z,
        gy271_status s
      )
      : heading(heading),
        x(x), y(y), z(z),
        status(s)
  {
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
  }

  float get_heading(){return heading;}
  int get_x(){return x;}
  int get_y(){return y;}
  int get_z(){return z;}
  gy271_status get_status(){return status;};
  struct tm* get_gmtime(){return gmtime(&ts.tv_sec);}
private:
  float heading;
  int x, y, z;

  gy271_status status;
  struct timespec ts;
};

class gy271_observer {
public:
  virtual ~gy271_observer() = default;
  virtual void update(gy271_event&) = 0;
};

class gy271_worker : public worker{
public:
  void init_gpio();
  void run();
  void add(gy271_observer& o);
  void remove(gy271_observer& o);

private:
  int fd;

  std::vector<gy271_observer*> observers;
  pthread_t thread_handler;
  pthread_mutex_t mutex;

  void update(float heading,
              int x, int y, int z,
              gy271_status s);
  int check_status(int fd, int status);
};

#endif






