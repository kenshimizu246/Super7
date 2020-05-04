
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#ifndef _njm2670d2_hpp
#define _njm2670d2_hpp


class njm2670d2 {
public:
  njm2670d2(unsigned int a1, unsigned int a2, unsigned int b1, unsigned int b2)
    : a1(a1), a2(a2), b1(b1), b2(b2)
  {
  }

  void initGpio(){
    wiringPiSetupGpio();
  }

  void initMode(){
    std::cout << "njm2670d2::initMode" << std::endl;
    pinMode(a1, OUTPUT);
    pinMode(b2, OUTPUT);
    pinMode(b1, OUTPUT);
    pinMode(b2, OUTPUT);

    digitalWrite(a1, LOW);
    digitalWrite(a2, LOW);
    digitalWrite(b1, LOW);
    digitalWrite(b2, LOW);
  }

  void stop(){
    digitalWrite(a1, LOW);
    digitalWrite(a2, LOW);
    digitalWrite(b1, LOW);
    digitalWrite(b2, LOW);
    std::cout << "njm2670d2::stop()!" << std::endl;
  }

  void forward(){
    digitalWrite(a1, HIGH);
    digitalWrite(a2, LOW);
    digitalWrite(b1, HIGH);
    digitalWrite(b2, LOW);
    std::cout << "njm2670d2::forward()!" << std::endl;
  }

  void backward(){
    digitalWrite(a1, LOW);
    digitalWrite(a2, HIGH);
    digitalWrite(b1, LOW);
    digitalWrite(b2, HIGH);
    std::cout << "njm2670d2::backward()!" << std::endl;
  }

  void right(){
    digitalWrite(a1, LOW);
    digitalWrite(a2, HIGH);
    digitalWrite(b1, HIGH);
    digitalWrite(b2, LOW);
    std::cout << "njm2670d2::right()!" << std::endl;
  }

  void left(){
    digitalWrite(a1, HIGH);
    digitalWrite(a2, LOW);
    digitalWrite(b1, LOW);
    digitalWrite(b2, HIGH);
    std::cout << "njm2670d2::left()!" << std::endl;
  }

private:
  unsigned int a1;
  unsigned int a2;
  unsigned int b1;
  unsigned int b2;
};

#endif
