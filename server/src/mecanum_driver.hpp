
#include "motor_driver.hpp"

#ifndef _mecanum_driver_hpp
#define _mecanum_driver_hpp

class mecanum_driver : public motor_observer {
public:
  mecanum_driver(
         uint16_t front_right_motor_1,
         uint16_t front_right_motor_2,
         uint16_t front_left_motor_1,
         uint16_t front_left_motor_2,
         uint16_t rear_right_motor_1,
         uint16_t rear_right_motor_2,
         uint16_t rear_left_motor_1,
         uint16_t rear_left_motor_2,
         uint16_t front_right_sensor_1,
         uint16_t front_right_sensor_2,
         uint16_t front_left_sensor_1,
         uint16_t front_left_sensor_2,
         uint16_t rear_right_sensor_1,
         uint16_t rear_right_sensor_2,
         uint16_t rear_left_sensor_1,
         uint16_t rear_left_sensor_2
  ) : fr_motor(0, front_right_motor_1,
                      front_right_motor_2,
                      front_right_sensor_1,
                      front_right_sensor_2),
      fl_motor(1, front_left_motor_1,
                      front_left_motor_2,
                      front_left_sensor_1,
                      front_left_sensor_2),
      rr_motor(2, rear_right_motor_1,
                      rear_right_motor_2,
                      rear_right_sensor_1,
                      rear_right_sensor_2),
      rl_motor(3, rear_left_motor_1,
                      rear_left_motor_2,
                      rear_left_sensor_1,
                      rear_left_sensor_2)
  {
    fr_motor.add((*this));
    fl_motor.add((*this));
    rr_motor.add((*this));
    rl_motor.add((*this));
  }

  ~mecanum_driver(){
    std::cout << "~mecanum_driver: end" << std::endl;
  }

  void update(motor_event& event){
    std::cout << "mecanum_driver:update:"
      << "[id:" << event.get_id() << "]"
      << "[count:" << event.get_count() << "]"
      << std::endl;
    if(stop_cnt > 0 && stop_cnt <= event.get_count()){
      stop();
    }
  }

  enum STATE {
    FORWARD,
    FORWARD_RIGHT,
    FORWARD_LEFT,
    BACKWARD,
    BACKWARD_RIGHT,
    BACKWARD_LEFT,
    ROUND_RIGHT,
    ROUND_LEFT,
    SLIDE_RIGHT,
    SLIDE_LEFT,
    STOP,
    UNKNOWN
  };

  STATE get_state(){
    motor_driver::STATE fr, fl, rr, rl;

    fr = fr_motor.get_state();
    fl = fl_motor.get_state();
    rr = rr_motor.get_state();
    rl = rl_motor.get_state();

    if(fr == motor_driver::STATE::FORWARD
       && fl == motor_driver::STATE::FORWARD
       && rr == motor_driver::STATE::FORWARD
       && rl == motor_driver::STATE::FORWARD){
      return FORWARD;
    } else if(fr == motor_driver::STATE::STOP
           && fl == motor_driver::STATE::FORWARD
           && rr == motor_driver::STATE::FORWARD
           && rl == motor_driver::STATE::STOP){
      return FORWARD_RIGHT;
    } else if(fr == motor_driver::STATE::FORWARD
           && fl == motor_driver::STATE::STOP
           && rr == motor_driver::STATE::STOP
           && rl == motor_driver::STATE::FORWARD){
      return FORWARD_LEFT;
    } else if(fr == motor_driver::STATE::BACKWARD
           && fl == motor_driver::STATE::BACKWARD
           && rr == motor_driver::STATE::BACKWARD
           && rl == motor_driver::STATE::BACKWARD){
      return BACKWARD;
    } else if(fr == motor_driver::STATE::BACKWARD
           && fl == motor_driver::STATE::STOP
           && rr == motor_driver::STATE::STOP
           && rl == motor_driver::STATE::BACKWARD){
      return BACKWARD_RIGHT;
    } else if(fr == motor_driver::STATE::STOP
           && fl == motor_driver::STATE::BACKWARD
           && rr == motor_driver::STATE::BACKWARD
           && rl == motor_driver::STATE::STOP){
      return BACKWARD_LEFT;
    } else if(fr == motor_driver::STATE::BACKWARD
           && fl == motor_driver::STATE::FORWARD
           && rr == motor_driver::STATE::BACKWARD
           && rl == motor_driver::STATE::FORWARD){
      return ROUND_RIGHT;
    } else if(fr == motor_driver::STATE::FORWARD
           && fl == motor_driver::STATE::BACKWARD
           && rr == motor_driver::STATE::FORWARD
           && rl == motor_driver::STATE::BACKWARD){
      return ROUND_RIGHT;
    } else if(fr == motor_driver::STATE::BACKWARD
           && fl == motor_driver::STATE::FORWARD
           && rr == motor_driver::STATE::FORWARD
           && rl == motor_driver::STATE::BACKWARD){
      return SLIDE_RIGHT;
    } else if(fr == motor_driver::STATE::FORWARD
           && fl == motor_driver::STATE::BACKWARD
           && rr == motor_driver::STATE::BACKWARD
           && rl == motor_driver::STATE::FORWARD){
      return SLIDE_LEFT;
    }

    return UNKNOWN;
  }

  bool is_forward(){
    if(FORWARD == get_state()
        || FORWARD_RIGHT == get_state()
        || FORWARD_LEFT == get_state()){
      return true;
    }
    return false;
  }

  bool is_backward(){
    if(BACKWARD == get_state()
        || BACKWARD_RIGHT == get_state()
        || BACKWARD_LEFT == get_state()){
      return true;
    }
    return false;
  }

  void init_mode(){
    fr_motor.init_mode();
    fl_motor.init_mode();
    rr_motor.init_mode();
    rl_motor.init_mode();
  }

  void start_monitor(){
    fr_motor.start_monitor();
    fl_motor.start_monitor();
    rr_motor.start_monitor();
    rl_motor.start_monitor();
  }

  void stop(){
    fr_motor.stop();
    fl_motor.stop();
    rr_motor.stop();
    rl_motor.stop();
    std::cout << "mecanum_driver:stop:"
      << "[stop_cnt:" << stop_cnt << "]"
      << std::endl;
    stop_cnt = 0;
  }

  void forward(){
    fr_motor.forward();
    fl_motor.forward();
    rr_motor.forward();
    rl_motor.forward();
  }

  void forward(uint64_t count){
    stop_cnt = count;
    fr_motor.forward(count);
    fl_motor.forward(count);
    rr_motor.forward(count);
    rl_motor.forward(count);
  }

  void forward_right(){
    fr_motor.stop();
    fl_motor.forward();
    rr_motor.forward();
    rl_motor.stop();
  }

  void forward_right(uint64_t count){
    stop_cnt = count;
    fr_motor.stop();
    fl_motor.forward(count);
    rr_motor.forward(count);
    rl_motor.stop();
  }

  void forward_left(){
    fr_motor.forward();
    fl_motor.stop();
    rr_motor.stop();
    rl_motor.forward();
  }

  void forward_left(uint64_t count){
    stop_cnt = count;
    fr_motor.forward(count);
    fl_motor.stop();
    rr_motor.stop();
    rl_motor.forward(count);
  }

  void backward(){
    fr_motor.backward();
    fl_motor.backward();
    rr_motor.backward();
    rl_motor.backward();
  }

  void backward(uint64_t count){
    stop_cnt = count;
    fr_motor.backward(count);
    fl_motor.backward(count);
    rr_motor.backward(count);
    rl_motor.backward(count);
  }

  void backward_right(){
    fr_motor.backward();
    fl_motor.stop();
    rr_motor.stop();
    rl_motor.backward();
  }

  void backward_right(uint64_t count){
    stop_cnt = count;
    fr_motor.backward(count);
    fl_motor.stop();
    rr_motor.stop();
    rl_motor.backward(count);
  }

  void backward_left(){
    fr_motor.stop();
    fl_motor.backward();
    rr_motor.backward();
    rl_motor.stop();
  }

  void backward_left(uint64_t count){
    stop_cnt = count;
    fr_motor.stop();
    fl_motor.backward(count);
    rr_motor.backward(count);
    rl_motor.stop();
  }

  void round_right(){
    fr_motor.backward();
    fl_motor.forward();
    rr_motor.backward();
    rl_motor.forward();
  }

  void round_right(uint64_t count){
    stop_cnt = count;
    fr_motor.backward(count);
    fl_motor.forward(count);
    rr_motor.backward(count);
    rl_motor.forward(count);
  }

  void round_left(){
    fr_motor.forward();
    fl_motor.backward();
    rr_motor.forward();
    rl_motor.backward();
  }

  void round_left(uint64_t count){
    stop_cnt = count;
    fr_motor.forward(count);
    fl_motor.backward(count);
    rr_motor.forward(count);
    rl_motor.backward(count);
  }

  void slide_right(){
    fr_motor.backward();
    fl_motor.forward();
    rr_motor.forward();
    rl_motor.backward();
  }

  void slide_right(uint64_t count){
    stop_cnt = count;
    fr_motor.backward(count);
    fl_motor.forward(count);
    rr_motor.forward(count);
    rl_motor.backward(count);
  }

  void slide_left(){
    fr_motor.forward();
    fl_motor.backward();
    rr_motor.backward();
    rl_motor.forward();
  }

  void slide_left(uint64_t count){
    stop_cnt = count;
    fr_motor.forward(count);
    fl_motor.backward(count);
    rr_motor.backward(count);
    rl_motor.forward(count);
  }

  void add(motor_observer& o){
    fr_motor.add(o);
    fl_motor.add(o);
    rr_motor.add(o);
    rl_motor.add(o);
  }

  void remove(motor_observer& o){
    fr_motor.remove(o);
    fl_motor.remove(o);
    rr_motor.remove(o);
    rl_motor.remove(o);
  }

private:
  motor_driver fr_motor;
  motor_driver fl_motor;
  motor_driver rr_motor;
  motor_driver rl_motor;
  uint64_t stop_cnt;
};

#endif
