
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>
#include <memory>

#include "pca9685.hpp"
#include "motor_driver.hpp"
#include "mecanum_driver.hpp"
#include "auto_drive/auto_drive.hpp"

using namespace std;

#ifndef _app_ctx_hpp
#define _app_ctx_hpp

namespace tamageta {

class app_ctx {
private:
  pca9685 *servo = nullptr;
  mecanum_driver *motor = nullptr;
  vl53l0x_worker *vl53l0x = nullptr;
  gy271_worker *gy271 = nullptr;
  auto_drive *autodrive = nullptr;

  app_ctx(){}
  ~app_ctx(){}

public:


  pca9685* get_pca9685(){
    return servo;
  }
  mecanum_driver* get_mecanum_driver(){
    return motor;
  }
  vl53l0x_worker* get_vl53l0x_worker(){
    return vl53l0x;
  }
  gy271_worker* get_gy271_worker(){
    return gy271;
  }
  auto_drive* get_auto_drive(){
    return autodrive;
  }

  void init(pca9685 *s,
            mecanum_driver *m,
            vl53l0x_worker *v,
            gy271_worker *g,
            auto_drive *a){
    servo = s;
    motor = m;
    vl53l0x = v;
    gy271 = g;
    autodrive = a;
  }
  static app_ctx& get_instance() {
    static app_ctx instance;
    return instance;
  }
};

}

#endif /*_app_ctx_hpp*/
