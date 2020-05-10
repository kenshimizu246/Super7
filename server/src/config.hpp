
#include <string>
#include <exception>
#include <stdexcept>

using namespace std;

#ifndef _config_hpp
#define _config_hpp

namespace tamageta {

class config {
  private:
    config();
    ~config();
    bool initialized;
    string appdir;
    string logdir;
    string pidfile;
    uint32_t port = 9009;
    bool daemon;

    uint16_t front_right_wheel_1;
    uint16_t front_right_wheel_2;
    uint16_t front_left_wheel_1;
    uint16_t front_left_wheel_2;

    uint16_t rear_right_wheel_1;
    uint16_t rear_right_wheel_2;
    uint16_t rear_left_wheel_1;
    uint16_t rear_left_wheel_2;

    uint16_t front_right_sensor_1;
    uint16_t front_right_sensor_2;
    uint16_t front_left_sensor_1;
    uint16_t front_left_sensor_2;

    uint16_t rear_right_sensor_1;
    uint16_t rear_right_sensor_2;
    uint16_t rear_left_sensor_1;
    uint16_t rear_left_sensor_2;

  public:
    void load(const char* filename);
    string get_app_dir();
    string get_log_dir();
    string get_pid_file();
    uint32_t get_port();
    bool is_initialized();

    bool is_daemon();

    uint16_t get_front_right_wheel_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_right_wheel_1;
    }
    uint16_t get_front_right_wheel_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_right_wheel_2;
    }
    uint16_t get_front_left_wheel_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_left_wheel_1;
    }
    uint16_t get_front_left_wheel_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_left_wheel_2;
    }

    uint16_t get_rear_right_wheel_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_right_wheel_1;
    }
    uint16_t get_rear_right_wheel_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_right_wheel_2;
    }
    uint16_t get_rear_left_wheel_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_left_wheel_1;
    }
    uint16_t get_rear_left_wheel_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_left_wheel_2;
    }

    uint16_t get_front_right_sensor_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_right_sensor_1;
    }
    uint16_t get_front_right_sensor_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_right_sensor_2;
    }
    uint16_t get_front_left_sensor_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_left_sensor_1;
    }
    uint16_t get_front_left_sensor_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return front_left_sensor_2;
    }

    uint16_t get_rear_right_sensor_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_right_sensor_1;
    }
    uint16_t get_rear_right_sensor_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_right_sensor_2;
    }
    uint16_t get_rear_left_sensor_1(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_left_sensor_1;
    }
    uint16_t get_rear_left_sensor_2(){
      if(!initialized) throw runtime_error("config has not yet been initialized!");
      return rear_left_sensor_2;
    }

    static config& get_instance() {
      static config config;
      return config;
    }
};

}

#endif /*_config_hpp*/
