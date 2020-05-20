#include "../vl53l0x/VL53L0X.hpp"

#include <chrono>
//#include <csignal>
//#include <exception>
//#include <iomanip>
//#include <iostream>
//#include <unistd.h>
#include <pthread.h>
//#include <set>
#include <memory>
#include <vector>
#include <algorithm>

#include "vl53l0x_worker.hpp"

void vl53l0x_worker::add(vl53l0x_observer& o){
  observers.push_back(&o);
}

void vl53l0x_worker::removex(vl53l0x_observer& o){
  observers.erase(std::remove(observers.begin(), observers.end(), &o));
}

void vl53l0x_worker::update(uint16_t d){
  vl53l0x_event e(id, d);
  for(auto* o : observers){
    o->update(e);
  }
}


uint16_t vl53l0x_worker::get_distance(){
  return distance;
}

void vl53l0x_worker::set_high_speed(bool b){
  if(b & high_accuracy){
    // throw exception
    // #error HIGH_SPEED and HIGH_ACCURACY cannot be both enabled at once!
  }
  high_speed = b;
}

void vl53l0x_worker::set_high_accuracy(bool b){
  if(b & high_speed){
    // throw exception
    // #error HIGH_SPEED and HIGH_ACCURACY cannot be both enabled at once!
  }
  high_accuracy = b;
}

void vl53l0x_worker::set_long_range(bool b){
  long_range = b;
}

void* vl53l0x_worker::execute_launcher(void* args){
  std::cout << "vl53l0x_worker::execute_launcher()... start" << std::endl;
  reinterpret_cast<vl53l0x_worker*>(args)->run();
  std::cout << "vl53l0x_worker::execute_launcher()... end" << std::endl;
}

void vl53l0x_worker::start() {
  std::cout << "vl53l0x_worker::start()..." << std::endl;
//  if((this->thread_handler) == NULL){
    std::cout << "vl53l0x_worker::start()... thread init" << std::endl;
    pthread_mutex_init(&(this->mutex), NULL);
    pthread_create(
      &(this->thread_handler),
      NULL,
      vl53l0x_worker::execute_launcher,
      this
    );
    std::cout << "vl53l0x_worker::start()... thread created" << std::endl;
    pthread_detach(this->thread_handler);
    std::cout << "vl53l0x_worker::start()... thread detached" << std::endl;
//  }else{
//    std::cout << "vl53l0x_worker::start()... thread_handler is not null" << std::endl;
//  }
}

void vl53l0x_worker::run() {
  std::cout << "vl53l0x_worker::run(): start..." << std::endl;

  VL53L0X sensor;
  try {
    sensor.initialize();
    sensor.setTimeout(200);
    std::cout << "vl53l0x_worker::run(): sendor initialized" << std::endl;
  } catch (const std::exception & error) {
    std::string m = "Error initializing sensor with reason: ";
    m += error.what();
    std::cerr << m << std::endl;
    throw vl53l0x_error(m.c_str());
  }

  if(long_range){
    try {
      // Lower the return signal rate limit (default is 0.25 MCPS)
      sensor.setSignalRateLimit(0.1);
      // Increase laser pulse periods (defaults are 14 and 10 PCLKs)
      sensor.setVcselPulsePeriod(VcselPeriodPreRange, 18);
      sensor.setVcselPulsePeriod(VcselPeriodFinalRange, 14);
    } catch (const std::exception & error) {
      std::string m = "Error enabling long range mode with reason: ";
      m += error.what();
      std::cerr << m << std::endl;
      throw vl53l0x_error(m.c_str());
    }
  }

  if(high_speed){
    try {
      // Reduce timing budget to 20 ms (default is about 33 ms)
      sensor.setMeasurementTimingBudget(20000);
    } catch (const std::exception & error) {
      std::string m = "Error enabling high speed mode with reason: ";
      m += error.what();
      std::cerr << m << std::endl;
      throw vl53l0x_error(m.c_str());
    }
  }else if(high_accuracy){
    try {
      // Increase timing budget to 200 ms
      sensor.setMeasurementTimingBudget(200000);
    } catch (const std::exception & error) {
      std::string m = "Error enabling high accuracy mode with reason: ";
      m += error.what();
      std::cerr << m << std::endl;
      throw vl53l0x_error(m.c_str());
    }
  }

  // Highly unprobable but check SIGINT exit flag
  if (stop) {
    std::cerr << "Stop Flag is true!" << std::endl;
    return;
  }

  // Durations in nanoseconds
  total_duration = 0;
  max_duration = 0;
  min_duration = 1000*1000*1000;
  // Initialize reference time measurement
  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

  std::cout << "vl53l0x_worker::run(): start loop" << std::endl;
  int cnt = 0;
  for (;!stop; ++cnt) {
    try {
      distance = sensor.readRangeSingleMillimeters();
      //std::cout << "vl53l0x_worker::run(): distance: " << distance << std::endl;
      update(distance);
    } catch (const std::exception & error) {
      //std::cerr << "Error geating measurement with reason:" << std::endl << error.what() << std::endl;
      // You may want to bail out here, depending on your application - error means issues on I2C bus read/write.
      // return 3;
      distance = DISTANCE_ERROR;
    }

    // Check IO timeout and print range information
    if (sensor.timeoutOccurred()) {
      //std::cout << "\rReading" << cnt << " | timeout!" << std::endl;
    } else {
      //std::cout << "\rReading" << cnt << " | " << distance << std::endl;
    }
    std::cout << std::flush;

    // Calculate duration of current iteration
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    uint64_t duration = (std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1)).count();
    // Save current time as reference for next iteration
    t1 = t2;
    // Add total measurements duration
    total_duration += duration;
    // Skip comparing first measurement against max and min as it's not a full iteration
    if (cnt == 0) {
      continue;
    }
    // Check and save max and min iteration duration
    if (duration > max_duration) {
      max_duration = duration;
    }
    if (duration < min_duration) {
      min_duration = duration;
    }
  }

  // Print duration data
  //std::cout << "\nMax duration: " << max_duration << "ns" << std::endl;
  //std::cout << "Min duration: " << min_duration << "ns" << std::endl;
  //std::cout << "Avg duration: " << total_duration/(i+1) << "ns" << std::endl;
  //std::cout << "Avg frequency: " << 1000000000/(total_duration/(i+1)) << "Hz" << std::endl;
}

vl53l0x_worker::~vl53l0x_worker(){

}


