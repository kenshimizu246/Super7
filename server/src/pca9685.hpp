
#include <stdexcept>
#include <vector>

#ifndef _pca9685_hpp
#define _pca9685_hpp

class pca9685_error : public std::runtime_error{
public:
  pca9685_error(const char *_message)
      : std::runtime_error(_message) {}
};

class pca9685_pwm_write_event {
public:
  pca9685_pwm_write_event(int pin, int on, int off): pin(pin), on(on), off(off) {}

  int getPin(){ return pin; }
  int getOn(){ return on; }
  int getOff(){ return off; }

private:
  int pin;
  int on;
  int off;
};

class pca9685_observer {
public:
  virtual ~pca9685_observer() = default;
  virtual void update(pca9685_pwm_write_event&) = 0;
};

class pca9685 {
public:
  void PwmSetup(const int i2cAddress/* = 0x40*/, float freq/* = 50*/);
  void PwmFreq(float freq);
  void PwmReset();
  void PwmWrite(int pin, int on, int off);
  void PwmRead(int pin, int *on, int *off);
  void PwmFullOn(int pin, int tf);
  void PwmFullOff(int pin, int tf);

  void add(pca9685_observer& o);
  void remove(pca9685_observer& o);

private:
  bool initialized = false;
  int fd;
  std::vector<pca9685_observer*> observers;

  int baseReq(int pin);
};

#endif



