
#include <iostream>
#include <sstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

//#include <time.h>

#include <ctime>
#include <iostream>
#include <locale>
#include <exception>
#include <typeinfo>


#include "worker/vl53l0x_worker.hpp"
#include "worker/hcsr04_worker.hpp"
#include "worker/gy271_worker.hpp"
#include "message.hpp"
#include "command.hpp"

using namespace rapidjson;

const char* message_handler::kHeader = "header";
const char* message_handler::kBody = "body";

const std::string message_handler::kFORWARD = "FORWARD";
const std::string message_handler::kBACKWARD = "BACKWARD";
const std::string message_handler::kRIGHT = "RIGHT";
const std::string message_handler::kLEFT = "LEFT";
const std::string message_handler::kSTOP = "STOP";
const std::string message_handler::kAUTO = "AUTO";



void message_handler::toString(std::time_t t, char* b){
  if(!std::strftime(b, sizeof(b), "%FT%T%z", std::localtime(&t))){
    std::cerr << "time error!" << std::endl;
  }
}

/*************************************
{ "header":{
    "message-type":"event",
    "message-version":1,
    "session-id":"abc123",
    "source": "vl53l0x"
    "source-id": "0"
  },
  "body":{
    "distance": 123,
    "status": "SUCCESS",
    "timestamp": "",
  }
}
*************************************/
void message_handler::toJSON(vl53l0x_event& event, std::string& s){
  Document d;
  // define the document as an object rather than an array
  d.SetObject();

  // create an allocator
  Document::AllocatorType& allocator = d.GetAllocator();

  Value header(kObjectType);
  header.AddMember("message-type", "event", allocator);
  header.AddMember("source", "vl53l0x", allocator);
  header.AddMember("source-id", event.getID(), allocator);
  d.AddMember("header", header, allocator);

  char buff[100];
  int len = strftime(buff, sizeof(buff), "%D %T", event.getGMTime());

  Value body(kObjectType);
  body.AddMember("distance", Value().SetFloat(event.getDistance()), allocator);
  body.AddMember("status", event.getStatus(), allocator);
  body.AddMember("timestamp", Value().SetString(buff, len, allocator), allocator);
  d.AddMember("body", body, allocator);

  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  d.Accept(writer);

  s.append(sb.GetString(), sb.GetLength());
  // std::cout << sb.GetString() << std::endl;
  //  const Ch* GetString() const {
  // actually char*
}

/*************************************
{ "header":{
    "message-type":"event",
    "message-version":1,
    "session-id":"abc123",
    "source": "hcsr04"
    "source-id": "0"
  },
  "body":{
    "distance": 123,
    "status": "SUCCESS",
    "timestamp": "",
  }
}
*************************************/
void message_handler::toJSON(hcsr04_event& event, std::string& s){
  Document d;
  // define the document as an object rather than an array
  d.SetObject();

  // create an allocator
  Document::AllocatorType& allocator = d.GetAllocator();

  Value header(kObjectType);
  header.AddMember("message-type", "event", allocator);
  header.AddMember("source", "hcsr04", allocator);
  header.AddMember("source-id", 0, allocator);
  d.AddMember("header", header, allocator);

  char buff[100];
  int len = strftime(buff, sizeof(buff), "%D %T", event.getGMTime());

  Value body(kObjectType);
  //body.AddMember("distance", event.getDistance(), allocator);
  body.AddMember("distance", Value().SetInt64(event.getDistance()), allocator);
  body.AddMember("status", event.getStatus(), allocator);
  body.AddMember("timestamp", Value().SetString(buff, len, allocator), allocator);
  d.AddMember("body", body, allocator);

  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  d.Accept(writer);

  s.append(sb.GetString(), sb.GetLength());
  // std::cout << sb.GetString() << std::endl;
  //  const Ch* GetString() const {
  // actually char*
}

/*************************************
{ "header":{
    "message-type":"event",
    "message-version":1,
    "session-id":"abc123",
    "source": "gy271"
    "source-id": "0"
  },
  "body":{
    "heading": 123,
    "x": 123,
    "y": 123,
    "z": 123,
    "status": "SUCCESS",
    "timestamp": "",
  }
}
*************************************/
void message_handler::toJSON(gy271_event& event, std::string& s){
  Document d;
  d.SetObject();

  // create an allocator
  Document::AllocatorType& allocator = d.GetAllocator();

  Value header(kObjectType);
  header.AddMember("message-type", "event", allocator);
  header.AddMember("source", "gy271", allocator);
  header.AddMember("source-id", 0, allocator);
  d.AddMember("header", header, allocator);

  char buff[100];
  int len = strftime(buff, sizeof(buff), "%D %T", event.getGMTime());

  Value body(kObjectType);
  body.AddMember("heading", Value().SetInt64(event.getHeading()), allocator);
  body.AddMember("x", Value().SetInt64(event.getX()), allocator);
  body.AddMember("y", Value().SetInt64(event.getY()), allocator);
  body.AddMember("z", Value().SetInt64(event.getZ()), allocator);
  body.AddMember("status", event.getStatus(), allocator);
  body.AddMember("timestamp", Value().SetString(buff, len, allocator), allocator);
  d.AddMember("body", body, allocator);

  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  d.Accept(writer);

  s.append(sb.GetString(), sb.GetLength());
  // std::cout << sb.GetString() << std::endl;
  //  const Ch* GetString() const {
  // actually char*
}


void message_handler::toJSON(servo_command_event& event, std::string& s){
  Document d;
  d.SetObject();

  // create an allocator
  Document::AllocatorType& allocator = d.GetAllocator();

  Value header(kObjectType);
  header.AddMember("message-type", "event", allocator);
  header.AddMember("source", "command", allocator);
  header.AddMember("source-id", 0, allocator);
  d.AddMember("header", header, allocator);

  char buff[100];
  int len = strftime(buff, sizeof(buff), "%D %T", event.getGMTime());

  Value body(kObjectType);
  body.AddMember("id", Value().SetInt64(event.getID()), allocator);
  body.AddMember("value", Value().SetInt64(event.getValue()), allocator);
  body.AddMember("timestamp", Value().SetString(buff, len, allocator), allocator);
  d.AddMember("body", body, allocator);

  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  d.Accept(writer);

  s.append(sb.GetString(), sb.GetLength());
}

void message_handler::toJSON(drive_command_event& event, std::string& s){
  Document d;
  d.SetObject();

  // create an allocator
  Document::AllocatorType& allocator = d.GetAllocator();

  Value header(kObjectType);
  header.AddMember("message-type", "event", allocator);
  header.AddMember("source", "command", allocator);
  header.AddMember("source-id", 0, allocator);
  d.AddMember("header", header, allocator);

  char buff[100];
  int len = strftime(buff, sizeof(buff), "%D %T", event.getGMTime());

  Value body(kObjectType);
  body.AddMember("type", Value().SetString(buff, len, allocator), allocator);
  body.AddMember("timestamp", Value().SetString(buff, len, allocator), allocator);
  d.AddMember("body", body, allocator);

  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  d.Accept(writer);

  s.append(sb.GetString(), sb.GetLength());
}

void message_handler::toJSON(command_event& event, std::string& s){
  Document d;
  d.SetObject();

  // create an allocator
  Document::AllocatorType& allocator = d.GetAllocator();

  Value header(kObjectType);
  header.AddMember("message-type", "event", allocator);
  header.AddMember("source", "command", allocator);
  header.AddMember("source-id", 0, allocator);
  d.AddMember("header", header, allocator);

  char buff[100];
  int len = strftime(buff, sizeof(buff), "%D %T", event.getGMTime());

  Value body(kObjectType);
  if(typeid(event) == typeid(servo_command_event&)){
    servo_command_event& ee = static_cast<servo_command_event&>(event);
    body.AddMember("id", Value().SetInt64(ee.getID()), allocator);
    body.AddMember("value", Value().SetInt64(ee.getValue()), allocator);
  } else if(typeid(event) == typeid(drive_command_event&)){
    drive_command_event& ee = static_cast<drive_command_event&>(event);
    //body.AddMember("type", Value().SetString(buff, len, allocator), allocator);
  }
  body.AddMember("timestamp", Value().SetString(buff, len, allocator), allocator);
  d.AddMember("body", body, allocator);

  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  d.Accept(writer);

  s.append(sb.GetString(), sb.GetLength());
}

/*************************************
{ "header":{
    "message-type":"command",
    "message-version":1,
    "session-id":"abc123",
    "target": "drive",
    "target-id": "0",
    "seq-id":0
  },
  "body":{ "command":"STOP"
  }
}

{ "header":{ "message-type":"command", "target": "drive", "target-id": "0" }, "body":{ "command":"STOP"}}
{ "header":{ "message-type":"command", "target": "drive", "target-id": "0" }, "body":{ "command":"FORWARD"}}
{ "header":{ "message-type":"command", "target": "drive", "target-id": "0" }, "body":{ "command":"BACKWARD"}}
{ "header":{ "message-type":"command", "target": "drive", "target-id": "0" }, "body":{ "command":"RIGHT"}}
{ "header":{ "message-type":"command", "target": "drive", "target-id": "0" }, "body":{ "command":"LEFT"}}

*************************************/
std::shared_ptr<command> message_handler::toCommand(command_factory& factory, std::string& s){
  Document d;
  std::cout << "toCommand: " << s << std::endl;

  if (d.Parse<0>(s.c_str()).HasParseError()){
    std::stringstream ss;
    ss << "Parse Error : " << s;
    throw message_handler_error(ss.str().c_str());
  }

  if (!d.HasMember(kHeader)){
    std::stringstream ss;
    ss << "No Header Error : " << s;
    throw message_handler_error(ss.str().c_str());
  }

  if (!d[kHeader].HasMember("target")){
    std::stringstream ss;
    ss << "No target tag Error : " << s;
    throw message_handler_error(ss.str().c_str());
  }
  std::string target = d[kHeader]["target"].GetString();

  if (!d.HasMember(kBody)){
    std::stringstream ss;
    ss << "No body tag Error : " << s;
    throw message_handler_error(ss.str().c_str());
  }

  std::shared_ptr<command> p;
  if(target == "drive") {
    if (!d[kBody].HasMember("command")){
      std::stringstream ss;
      ss << "No command tag Error : " << s;
      throw message_handler_error(ss.str().c_str());
    }
    std::string command = d[kBody]["command"].GetString();
    p = factory.createDriveCommand(command);
  } else if(target == "servo") {
    unsigned int id = d[kBody]["id"].GetInt();
    unsigned int value = d[kBody]["value"].GetInt();

    p = factory.createServoCommand(id, value);
  }
  return p;
}
