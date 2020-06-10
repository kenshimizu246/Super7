#include <chrono>
#include <pthread.h>
#include <memory>
#include <vector>
#include <algorithm>

#include <sys/time.h>

#include <iostream>
#include <ctime>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <math.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "gy271_worker.hpp"

void gy271_worker::add(gy271_observer& o){
  observers.push_back(&o);
}

void gy271_worker::remove(gy271_observer& o){
  observers.erase(std::remove(observers.begin(), observers.end(), &o));
}

void gy271_worker::update(
        float heading,
        int x, int y, int z,
        gy271_status s
      ) {
  if(!get_stop()){
    gy271_event e(
      heading,
      x, y, z,
      s);
    for(auto* o : observers){
      o->update(e);
    }
  }
}

int gy271_worker::check_status(int fd, int status){
  // DRDY: “0”: no new data, “1”: new data is ready
  if((status & 0x01) > 0){
    //printf("GY-271: new data is ready!\n");
  } else {
    printf("GY-271: no new data!\n");
    return -1;
  }

  // OVL: “0”: normal, “1”: data overflow
  if((status & 0x02) > 0){
    printf("GY-271: data overflow!\n");
    return -2;
  }

  // DOR: “0”: normal, “1”: data skipped for reading
  if((status & 0x04) > 0){
    printf("GY-271: data skipped for reading!\n");
    return -3;
  }
  return 0;
}

void gy271_worker::init_gpio(){
  wiringPiSetupGpio();
}

void gy271_worker::run(){
  printf("GY-271: GY-271\n");

  // Check i2c address
  int fdi = wiringPiI2CSetup(GY_271_ADDR);
  if (fd < 0){
    throw gy271_error("wiringPiI2CSetup error!");
  }
  fd = fdi;

  int ret = wiringPiI2CWriteReg8(fd, 0x0b, 0x01);
  if(ret < 0){
    throw gy271_error("Error : Reset failed! writing 0x01 to 0x0b!");
  }

  ret = wiringPiI2CWriteReg8(fd, 0x0a, 0x80);
  if(ret < 0){
    throw gy271_error("Error : Soft Reset failed! writing 0x80 to 0x0a");
  }

  //int ctrl = MODE_CONTINUOUS | ODR_200HZ | RNG_8G | OSR_512;
  int ctrl = MODE_CONTINUOUS | ODR_10HZ | RNG_8G | OSR_256;
  printf("GY-271: ctrl %x\n", ctrl);
  ret = wiringPiI2CWriteReg8(fd, 0x09, ctrl);
  if(ret < 0){
    //throw gy271_error("Error %d\n", ret);
    throw gy271_error("Error writing 0x09");
  }

  unsigned int lowX, highX, lowY, highY, lowZ, highZ;
  lowX = lowY = lowZ = 0xffff;
  highX = highY = highZ = 0;

  while(!get_stop()){
    int status;
    //printf("GY-271: start and reading status\n");
    do{
      status = wiringPiI2CReadReg8(fd, 0x06);
    } while((status & 0x01) != 1 && !get_stop());
    //printf("GY-271 will check the status\n");
    // DRDY: “0”: no new data, “1”: new data is ready
    if((status & 0x01) > 0){
      //printf("GY-271: new data is ready!\n");
    } else {
      printf("GY-271: no new data!\n");
      update(-1, -1, -1, -1, gy271_status::GY271_NO_NEW_DATA);
      continue;
    }

    // OVL: “0”: normal, “1”: data overflow
    if((status & 0x02) > 0){
      //printf("GY-271: data overflow!\n");
      printf("o");
      //update(-1, -1, -1, -1, gy271_status::GY271_DATA_OVERFLOW);
      //continue;
    }

    // DOR: “0”: normal, “1”: data skipped for reading
    if((status & 0x04) > 0){
      //printf("GY-271: Data skipped for reading!\n");
      printf("s");
      continue;
    } else {
      //printf("\nGY-271: Data skip (DOR) bit is normal!\n");
      printf(",\n");
    }
    int lsbX = wiringPiI2CReadReg8(fd, 0x00);
    int msbX = wiringPiI2CReadReg8(fd, 0x01);
    int lsbY = wiringPiI2CReadReg8(fd, 0x02);
    int msbY = wiringPiI2CReadReg8(fd, 0x03);
    int lsbZ = wiringPiI2CReadReg8(fd, 0x04);
    int msbZ = wiringPiI2CReadReg8(fd, 0x05);
    int x = lsbX | (msbX << 8);
    int y = lsbY | (msbY << 8);
    int z = lsbZ | (msbZ << 8);

    if(x < lowX) lowX = x;
    if(x > highX) highX = x;
    if(y < lowY) lowY = y;
    if(y > highY) highY = y;
    if(z < lowZ) lowZ = z;
    if(z > highZ) highZ = z;
    if(lowX == highX || lowY == highY){
      update(-1,
        x, y, z,
        gy271_status::GY271_NOT_ENOUGH);
      continue;
    }
    int ofsX = (highX + lowX)/2;
    int ofsY = (highY + lowY)/2;
    int ofsZ = (highZ + lowZ)/2;

    x -= ofsX;
    y -= ofsY;
    z -= ofsZ;

    float fx = (float)x/(highX - lowX);
    float fy = (float)y/(highY - lowY);
    float fz = (float)z/(highZ - lowZ);

    float heading = atan2(fy, fx) * 180.0 / 3.1415926535;
    if(heading <= 0){heading += 360;}

    update(heading,
        x, y, z,
        gy271_status::GY271_SUCCESS);
    //usleep(1000000); //1sec
  }
}

