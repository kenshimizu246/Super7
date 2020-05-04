
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "command.hpp"

using namespace rapidjson;

#ifndef _message_handler_hpp
#define _message_handler_hpp

class message_handler_error : public std::runtime_error {
public:
  message_handler_error(const char *_message)
      : runtime_error(_message) {}
};

class message_handler{
public:
  static void toJSON(vl53l0x_event& event, std::string& s);
  static void toJSON(hcsr04_event& event, std::string& s);
  static void toJSON(gy271_event& event, std::string& s);
  static void toJSON(servo_command_event& event, std::string& s);
  static void toJSON(drive_command_event& event, std::string& s);
  static void toJSON(command_event& event, std::string& s);
  static std::shared_ptr<command> toCommand(command_factory& factory, std::string& s);

  static std::shared_ptr<drive_command> toDriveCommand(std::string& s);
  static std::shared_ptr<servo_command> toServoCommand(std::string& s);

  static const char *kHeader;
  static const char *kBody;

  static const std::string kFORWARD;
  static const std::string kBACKWARD;
  static const std::string kRIGHT;
  static const std::string kLEFT;
  static const std::string kSTOP;
  static const std::string kAUTO;

private:
  void toString(std::time_t t, char* b);
};

#endif

