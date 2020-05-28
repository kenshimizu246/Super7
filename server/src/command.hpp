
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include "config.hpp"
#include "app_ctx.hpp"
#include "pca9685.hpp"
#include "motor_driver.hpp"
#include "mecanum_driver.hpp"
#include "auto_drive/auto_drive.hpp"

using namespace tamageta;

#ifndef _command_hpp
#define _command_hpp


enum drive_command_type {
  FORWARD,
  BACKWARD,
  RIGHT,
  LEFT,
  STOP,
  FORWARD_RIGHT,
  FORWARD_LEFT,
  BACKWARD_RIGHT,
  BACKWARD_LEFT,
  SLIDE_RIGHT,
  SLIDE_LEFT,
  AUTO,
  UNKNOWN
};

class command_event {
public:
  struct timespec getTime(){ return ts; }
  struct tm* getGMTime(){ return gmtime(&ts.tv_sec); }
private:
  struct timespec ts;
};

class command_observer {
public:
  virtual ~command_observer() = default;
  virtual void update(command_event&) = 0;
};

class command {
public:
  virtual void doCommand() = 0;

  void add(command_observer& o){
    observers.push_back(&o);
  }
  void remove(command_observer& o){
    observers.erase(std::remove(observers.begin(), observers.end(), &o));
  }
  void update(command_event& e){
    for(auto* o : observers){
//      o->update(e);
    }
  }
private:
  std::vector<command_observer*> observers;
};

class drive_command_event : public command_event {
public:
  drive_command_event(drive_command_type t): type(t) {}
  drive_command_type getType(){ return type; }
private:
  drive_command_type type;
};

class drive_command : public command {
public:
  const std::string kFORWARD = "FORWARD";
  const std::string kBACKWARD = "BACKWARD";
  const std::string kRIGHT = "RIGHT";
  const std::string kLEFT = "LEFT";
  const std::string kSTOP = "STOP";
  const std::string kAUTO = "AUTO";
  const std::string kFORWARD_RIGHT = "FORWARD_RIGHT";
  const std::string kFORWARD_LEFT = "FORWARD_LEFT";
  const std::string kBACKWARD_RIGHT = "BACKWARD_RIGHT";
  const std::string kBACKWARD_LEFT = "BACKWARD_LEFT";
  const std::string kSLIDE_RIGHT = "SLIDE_RIGHT";
  const std::string kSLIDE_LEFT = "SLIDE_LEFT";

  drive_command(mecanum_driver& m, drive_command_type s, pca9685& p, vl53l0x_worker& v, gy271_worker& g)
          : motor(m), type(s), count(0), servo(p), vl53l0x(v), gy271(g) {}
  drive_command(mecanum_driver& m, drive_command_type s, uint64_t c, pca9685& p, vl53l0x_worker& v, gy271_worker& g)
          : motor(m), type(s), count(c), servo(p), vl53l0x(v), gy271(g) {}
  drive_command(mecanum_driver& m, std::string& s, pca9685& p, vl53l0x_worker& v, gy271_worker& g)
          : motor(m), count(0), servo(p), vl53l0x(v), gy271(g) {
    setType(s);
  }
  drive_command(mecanum_driver& m, std::string& s, uint64_t c, pca9685& p, vl53l0x_worker& v, gy271_worker& g)
          : motor(m), count(c), servo(p), vl53l0x(v), gy271(g) {
    setType(s);
  }

  void setType(std::string& str){
    if (str == kFORWARD) {
      type = drive_command_type::FORWARD;
    } else if (str == kBACKWARD) {
      type = drive_command_type::BACKWARD;
    } else if (str == kRIGHT) {
      type = drive_command_type::RIGHT;
    } else if (str == kLEFT) {
      type = drive_command_type::LEFT;
    } else if (str == kSTOP) {
      type = drive_command_type::STOP;
    } else if (str == kFORWARD_RIGHT) {
      type = drive_command_type::FORWARD_RIGHT;
    } else if (str == kFORWARD_LEFT) {
      type = drive_command_type::FORWARD_LEFT;
    } else if (str == kBACKWARD_RIGHT) {
      type = drive_command_type::BACKWARD_RIGHT;
    } else if (str == kBACKWARD_LEFT) {
      type = drive_command_type::BACKWARD_LEFT;
    } else if (str == kSLIDE_RIGHT) {
      type = drive_command_type::SLIDE_RIGHT;
    } else if (str == kSLIDE_LEFT) {
      type = drive_command_type::SLIDE_LEFT;
    } else if (str == kAUTO) {
      type = drive_command_type::AUTO;
    } else {
      std::stringstream ss;
      ss << "No supported command Error : " << str;
      //throw message_handler_error(ss.str().c_str());
    }
    drive_command_event e(type);
    update(e);
  }

  drive_command_type getType(){ return type; }

  void doCommand(){
    std::cout << "doCommand "
              << "[type:" << getType() << "]"
              << "[count:" << count << "]"
              << std::endl;
    if (getType() == drive_command_type::FORWARD){
      std::cout << "forward selected! "
        << "[count:" << count << "]"
        << "[servo:" << config::get_instance().get_front_vl53l0x_servo_mid() << "]"
        << std::endl;
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        config::get_instance().get_front_vl53l0x_servo_mid()
      );
      if(count > 0){
        motor.forward(count);
      } else {
        motor.forward();
      }
    }else if (getType() == drive_command_type::BACKWARD){
      std::cout << "backward selected!" << std::endl;
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        config::get_instance().get_front_vl53l0x_servo_mid()
      );
      if(count > 0){
        motor.backward(count);
      } else {
        motor.backward();
      }
    }else if (getType() == drive_command_type::RIGHT){
      std::cout << "right selected!" << std::endl;
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        config::get_instance().get_front_vl53l0x_servo_mid()
      );
      if(count > 0){
        motor.round_right(count);
      } else {
        motor.round_right();
      }
    }else if (getType() == drive_command_type::LEFT){
      std::cout << "left selected!" << std::endl;
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        config::get_instance().get_front_vl53l0x_servo_mid()
      );
      if(count > 0){
        motor.round_left(count);
      } else {
        motor.round_left();
      }
    }else if (getType() == drive_command_type::STOP){
      std::cout << "stop selected!" << std::endl;
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        config::get_instance().get_front_vl53l0x_servo_mid()
      );
      motor.stop();
    }else if (getType() == drive_command_type::FORWARD_RIGHT){
      std::cout << "forward right selected!" << std::endl;
      uint16_t max = config::get_instance().get_front_vl53l0x_servo_max();
      uint16_t min = config::get_instance().get_front_vl53l0x_servo_min();
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        ((max - min) * 0.25 + min)
      );
      if(count > 0){
        motor.forward_right(count);
      } else {
        motor.forward_right();
      }
    }else if (getType() == drive_command_type::FORWARD_LEFT){
      std::cout << "forward left selected!" << std::endl;
      uint16_t max = config::get_instance().get_front_vl53l0x_servo_max();
      uint16_t min = config::get_instance().get_front_vl53l0x_servo_min();
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        ((max - min) * 0.75 + min)
      );
      if(count > 0){
        motor.forward_left(count);
      } else {
        motor.forward_left();
      }
    }else if (getType() == drive_command_type::BACKWARD_RIGHT){
      std::cout << "backward right selected!" << std::endl;
      uint16_t max = config::get_instance().get_front_vl53l0x_servo_max();
      uint16_t min = config::get_instance().get_front_vl53l0x_servo_min();
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        ((max - min) * 0.75 + min)
      );
      if(count > 0){
        motor.backward_right(count);
      } else {
        motor.backward_right();
      }
    }else if (getType() == drive_command_type::BACKWARD_LEFT){
      std::cout << "backward left selected!" << std::endl;
      uint16_t max = config::get_instance().get_front_vl53l0x_servo_max();
      uint16_t min = config::get_instance().get_front_vl53l0x_servo_min();
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        ((max - min) * 0.25 + min)
      );
      if(count > 0){
        motor.backward_left(count);
      } else {
        motor.backward_left();
      }
    }else if (getType() == drive_command_type::SLIDE_RIGHT){
      std::cout << "slide right selected!" << std::endl;
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        config::get_instance().get_front_vl53l0x_servo_min()
      );
      if(count > 0){
        motor.slide_right(count);
      } else {
        motor.slide_right();
      }
    }else if (getType() == drive_command_type::SLIDE_LEFT){
      std::cout << "slide left selected!" << std::endl;
      servo.PwmWrite(
        config::get_instance().get_front_vl53l0x_servo_id(),
        0,
        config::get_instance().get_front_vl53l0x_servo_max()
      );
      if(count > 0){
        motor.slide_left(count);
      } else {
        motor.slide_left();
      }
    }else if (getType() == drive_command_type::AUTO){
      std::cout << "auto selected!" << std::endl;
      shared_ptr<drive_action> check_action = shared_ptr<check_and_forward_action>(new check_and_forward_action(motor, servo, vl53l0x, gy271));
      shared_ptr<drive_action> move_action = shared_ptr<drive_action>(new move_forward_action(motor, servo, vl53l0x, gy271, mecanum_driver::STATE::UNKNOWN, 100));

      check_action->set_next_action(move_action);
      move_action->set_next_action(check_action);

      std::cout << "add auto action!" << std::endl;
      app_ctx::get_instance().get_auto_drive()->add(check_action);
      std::cout << "added auto action!" << std::endl;

      //shared_ptr<drive_action> action = check_action;
      //while(action != nullptr){
      //  action->do_action();
      //  action = action->get_next_action();
      //}      
    }
  }

private:
  mecanum_driver& motor;
  drive_command_type type;

  uint64_t count;
  pca9685& servo;
  vl53l0x_worker& vl53l0x;
  gy271_worker& gy271;
};

class servo_command_event : public command_event {
public:
  servo_command_event(unsigned int id, unsigned int value): id(id), value(value) { }
  unsigned int getID(){ return id; }
  unsigned int getValue(){ return value; }

private:
  unsigned int id;
  unsigned int value;
};

class servo_command : public command {
public:
  servo_command(pca9685& s, unsigned int id, int value): servo(s), id(id), value(value) {}
  unsigned int getId() { return id;}
  unsigned int getValue() {return value; }
  void doCommand(){
    std::cout << "servo.PwmWrite(" << id << ":" << value << ")" << std::endl;
    servo.PwmWrite(id, 0, value);

    servo_command_event e(id, value);
    update(e);
  }

private:
  pca9685& servo;
  unsigned int id;
  unsigned int value;
};

class command_factory {
public:
  command_factory(pca9685& s, mecanum_driver& m, vl53l0x_worker& v, gy271_worker& g) 
        : servo(s), motor(m), vl53l0x(v), gy271(g) {}
  void set(pca9685& s){
    servo = s;
  }
  void set(mecanum_driver& m){
    motor = m;
  }
  std::shared_ptr<command> createDriveCommand(std::string& s){
    return std::shared_ptr<command>(new drive_command(motor, s, servo, vl53l0x, gy271));
  }
  std::shared_ptr<command> createDriveCommand(std::string& s, uint64_t count){
    return std::shared_ptr<command>(new drive_command(motor, s, count, servo, vl53l0x, gy271));
  }
  std::shared_ptr<command> createDriveCommand(drive_command_type s){
    return std::shared_ptr<command>(new drive_command(motor, s, servo, vl53l0x, gy271));
  }
  std::shared_ptr<command> createDriveCommand(drive_command_type s, uint64_t count){
    return std::shared_ptr<command>(new drive_command(motor, s, count, servo, vl53l0x, gy271));
  }
  std::shared_ptr<command> createServoCommand(int id, unsigned int value){
    return std::shared_ptr<command>(new servo_command(servo, id, value));
  }
private:
  pca9685& servo;
  mecanum_driver& motor;
  vl53l0x_worker& vl53l0x;
  gy271_worker& gy271;
};

#endif

