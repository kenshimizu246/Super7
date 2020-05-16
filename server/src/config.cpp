
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "config.hpp"

using namespace std;
using namespace rapidjson;

namespace tamageta {


config::config() {
  initialized = false;
}

config::~config() {
}

void config::load(const char* filename){
  ifstream ifs(filename);
  string ss, str;
  Document doc;

  const char *cpidfile = "pidfile";
  const char *clogdir = "logdir";
  const char *cappdir = "appdir";
  const char *cport = "port";

  const char *cDaemon = "daemon";

  const char *cfront_vl53l0x = "front_vl53l0x";
  const char *cfront_vl53l0x_address = "address";
  const char *cfront_vl53l0x_stop_distance = "stop_distance";

  const char *cmotor_driver = "motor_driver";

  const char *cfront_right_wheel_1 = "front_right_wheel_1";
  const char *cfront_right_wheel_2 = "front_right_wheel_2";
  const char *cfront_left_wheel_1 = "front_left_wheel_1";
  const char *cfront_left_wheel_2 = "front_left_wheel_2";

  const char *crear_right_wheel_1 = "rear_right_wheel_1";
  const char *crear_right_wheel_2 = "rear_right_wheel_2";
  const char *crear_left_wheel_1 = "rear_left_wheel_1";
  const char *crear_left_wheel_2 = "rear_left_wheel_2";

  const char *cfront_right_sensor_1 = "front_right_sensor_1";
  const char *cfront_right_sensor_2 = "front_right_sensor_2";
  const char *cfront_left_sensor_1 = "front_left_sensor_1";
  const char *cfront_left_sensor_2 = "front_left_sensor_2";

  const char *crear_right_sensor_1 = "rear_right_sensor_1";
  const char *crear_right_sensor_2 = "rear_right_sensor_2";
  const char *crear_left_sensor_1 = "rear_left_sensor_1";
  const char *crear_left_sensor_2 = "rear_left_sensor_2";

  if (ifs.fail()) {
    throw runtime_error("file read failed!");
  }

  while (getline(ifs, ss)) {
    cout << "[" << ss << "]" << endl;
    str += ss;
  }
  cout << "str[" << str << "]" << endl;

  if (doc.Parse<0>(str.c_str()).HasParseError()) {
    throw runtime_error("configuration parse error!");
  }

  // pidfile
  if (!doc.HasMember(cpidfile)) {
    throw runtime_error("pidfile is not configured!");
  }
  if (!doc[cpidfile].IsString()) {
    throw runtime_error("pidfile must be string!");
  }
  pidfile = doc[cpidfile].GetString();

  // appdir
  if (!doc.HasMember(cappdir)) {
    throw runtime_error("appdir is not configured!");
  }
  if (!doc[cappdir].IsString()) {
    throw runtime_error("appdir must be string!");
  }
  appdir = doc[cappdir].GetString();

  // logdir
  if (!doc.HasMember(clogdir)) {
    throw runtime_error("logdir is not configured!");
  }
  if (!doc[clogdir].IsString()) {
    throw runtime_error("logdir must be string!");
  }
  logdir = doc[clogdir].GetString();

  // port
  if (!doc.HasMember(cport)) {
    throw runtime_error("port is not configured!");
  }
  if (!doc[cport].IsInt()) {
    throw runtime_error("port must be integer!");
  }
  port = doc[cport].GetInt();

  // daemon
  daemon = false;
  if (!doc.HasMember(cDaemon)) {
    throw runtime_error("daemon is not configured!");
  }
  if (!doc[cDaemon].IsBool()) {
    throw runtime_error("daemon must be bool!");
  }
  daemon = doc[cDaemon].GetBool();
  if (!doc.HasMember(cDaemon)) {
    throw runtime_error("daemon is not configured!");
  }

  // front vl53l0x i2c
  if (!doc.HasMember(cfront_vl53l0x)) {
    throw runtime_error("front_vl53l0x is not configured!");
  }
  const Value& front_vl53l0x = doc[cfront_vl53l0x];

  // front vl53l0x address
  //if (!front_vl53l0x.HasMember(cfront_vl53l0x_address)) {
  //  throw runtime_error("front_vl53l0x.address is not configured!");
  //}
  //if (!front_vl53l0x[cfront_vl53l0x_address].IsInt()) {
  //  throw runtime_error("front_vl53l0x_address must be number (0x...)!");
  //}
  //front_vl53l0x_address = front_vl53l0x[cfront_vl53l0x_address].GetInt();

  // front vl53l0x stop distance
  if (!front_vl53l0x.HasMember(cfront_vl53l0x_stop_distance)) {
    throw runtime_error("front_vl53l0x_address_stop_distance is not configured!");
  }
  if (!front_vl53l0x[cfront_vl53l0x_stop_distance].IsInt()) {
    throw runtime_error("front_vl53l0x_stop_distance must be integer!");
  }
  front_vl53l0x_stop_distance = front_vl53l0x[cfront_vl53l0x_stop_distance].GetInt();


  // motor driver
  if (!doc.HasMember(cmotor_driver)) {
    throw runtime_error("motor driver is not configured!");
  }
  const Value& mdrv = doc[cmotor_driver];

  // Front Right Wheel
  if (!mdrv.HasMember(cfront_right_wheel_1)) {
    throw runtime_error("front_right_wheel_1 is not configured!");
  }
  if (!mdrv[cfront_right_wheel_1].IsInt()) {
    throw runtime_error("front_right_wheel_1 must be integer!");
  }
  front_right_wheel_1 = mdrv[cfront_right_wheel_1].GetInt();

  if (!mdrv.HasMember(cfront_right_wheel_2)) {
    throw runtime_error("front_right_wheel_2 is not configured!");
  }
  if (!mdrv[cfront_right_wheel_2].IsInt()) {
    throw runtime_error("front_right_wheel_2 must be integer!");
  }
  front_right_wheel_2 = mdrv[cfront_right_wheel_2].GetInt();

  // Front Left Wheel
  if (!mdrv.HasMember(cfront_left_wheel_1)) {
    throw runtime_error("front_left_wheel_1 is not configured!");
  }
  if (!mdrv[cfront_left_wheel_1].IsInt()) {
    throw runtime_error("front_left_wheel_1 must be integer!");
  }
  front_left_wheel_1 = mdrv[cfront_left_wheel_1].GetInt();

  if (!mdrv.HasMember(cfront_left_wheel_2)) {
    throw runtime_error("front_left_wheel_2 is not configured!");
  }
  if (!mdrv[cfront_left_wheel_2].IsInt()) {
    throw runtime_error("front_left_wheel_2 must be integer!");
  }
  front_left_wheel_2 = mdrv[cfront_left_wheel_2].GetInt();

  // Rear Right Wheel
  if (!mdrv.HasMember(crear_right_wheel_1)) {
    throw runtime_error("rear_right_wheel_1 is not configured!");
  }
  if (!mdrv[crear_right_wheel_1].IsInt()) {
    throw runtime_error("rear_right_wheel_1 must be integer!");
  }
  rear_right_wheel_1 = mdrv[crear_right_wheel_1].GetInt();

  if (!mdrv.HasMember(crear_right_wheel_2)) {
    throw runtime_error("rear_right_wheel_2 is not configured!");
  }
  if (!mdrv[crear_right_wheel_2].IsInt()) {
    throw runtime_error("rear_right_wheel_2 must be integer!");
  }
  rear_right_wheel_2 = mdrv[crear_right_wheel_2].GetInt();

  // Rear Left Wheel
  if (!mdrv.HasMember(crear_left_wheel_1)) {
    throw runtime_error("rear_left_wheel_1 is not configured!");
  }
  if (!mdrv[crear_left_wheel_1].IsInt()) {
    throw runtime_error("rear_left_wheel_1 must be integer!");
  }
  rear_left_wheel_1 = mdrv[crear_left_wheel_1].GetInt();

  if (!mdrv.HasMember(crear_left_wheel_2)) {
    throw runtime_error("rear_left_wheel_2 is not configured!");
  }
  if (!mdrv[crear_left_wheel_2].IsInt()) {
    throw runtime_error("rear_left_wheel_2 must be integer!");
  }
  rear_left_wheel_2 = mdrv[crear_left_wheel_2].GetInt();


  // Front Right Wheel Sensor
  if (!mdrv.HasMember(cfront_right_sensor_1)) {
    throw runtime_error("front_right_sensor_1 is not configured!");
  }
  if (!mdrv[cfront_right_sensor_1].IsInt()) {
    throw runtime_error("front_right_sensor_1 must be integer!");
  }
  front_right_sensor_1 = mdrv[cfront_right_sensor_1].GetInt();

  if (!mdrv.HasMember(cfront_right_sensor_2)) {
    throw runtime_error("front_right_sensor_2 is not configured!");
  }
  if (!mdrv[cfront_right_sensor_2].IsInt()) {
    throw runtime_error("front_right_sensor_2 must be integer!");
  }
  front_right_sensor_2 = mdrv[cfront_right_sensor_2].GetInt();

  // Front Left Wheel Sensor
  if (!mdrv.HasMember(cfront_left_sensor_1)) {
    throw runtime_error("front_left_sensor_1 is not configured!");
  }
  if (!mdrv[cfront_left_sensor_1].IsInt()) {
    throw runtime_error("front_left_sensor_1 must be integer!");
  }
  front_left_sensor_1 = mdrv[cfront_left_sensor_1].GetInt();

  if (!mdrv.HasMember(cfront_left_sensor_2)) {
    throw runtime_error("front_left_sensor_2 is not configured!");
  }
  if (!mdrv[cfront_left_sensor_2].IsInt()) {
    throw runtime_error("front_left_sensor_2 must be integer!");
  }
  front_left_sensor_2 = mdrv[cfront_left_sensor_2].GetInt();

  // Rear Right Wheel Sensor
  if (!mdrv.HasMember(crear_right_sensor_1)) {
    throw runtime_error("rear_right_sensor_1 is not configured!");
  }
  if (!mdrv[crear_right_sensor_1].IsInt()) {
    throw runtime_error("rear_right_sensor_1 must be integer!");
  }
  rear_right_sensor_1 = mdrv[crear_right_sensor_1].GetInt();

  if (!mdrv.HasMember(crear_right_sensor_2)) {
    throw runtime_error("rear_right_sensor_2 is not configured!");
  }
  if (!mdrv[crear_right_sensor_2].IsInt()) {
    throw runtime_error("rear_right_sensor_2 must be integer!");
  }
  rear_right_sensor_2 = mdrv[crear_right_sensor_2].GetInt();

  // Rear Left Wheel Sensor
  if (!mdrv.HasMember(crear_left_sensor_1)) {
    throw runtime_error("rear_left_sensor_1 is not configured!");
  }
  if (!mdrv[crear_left_sensor_1].IsInt()) {
    throw runtime_error("rear_left_sensor_1 must be integer!");
  }
  rear_left_sensor_1 = mdrv[crear_left_sensor_1].GetInt();

  if (!mdrv.HasMember(crear_left_sensor_2)) {
    throw runtime_error("rear_left_sensor_2 is not configured!");
  }
  if (!mdrv[crear_left_sensor_2].IsInt()) {
    throw runtime_error("rear_left_sensor_2 must be integer!");
  }
  rear_left_sensor_2 = mdrv[crear_left_sensor_2].GetInt();

  initialized = true;
  cout << "loaded!" << endl;
}

string config::get_pid_file() {
  if (!initialized) {
    throw runtime_error("not initialized!");
  }
  return pidfile;
}

string config::get_app_dir() {
  if (!initialized) {
    throw runtime_error("not initialized!");
  }
  return appdir;
}

string config::get_log_dir() {
  if (!initialized) {
    throw runtime_error("not initialized!");
  }
  return logdir;
}

uint32_t config::get_port() {
  if (!initialized) {
    throw runtime_error("not initialized!");
  }
  return port;
}

bool config::is_daemon() {
  if (!initialized) {
    throw runtime_error("not initialized!");
  }
  return daemon;
}

bool config::is_initialized() {
  return initialized;
}

}

