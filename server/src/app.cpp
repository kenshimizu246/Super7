
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <string>
#include <vector>
#include <memory>

#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include <set>

/*#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>*/
#include <websocketpp/common/thread.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

#include "config.hpp"
#include "worker/vl53l0x_worker.hpp"
#include "worker/hcsr04_worker.hpp"
#include "worker/gy271_worker.hpp"
#include "message.hpp"
#include "pca9685.hpp"
#include "mecanum_driver.hpp"
#include "command.hpp"

#include <wiringPi.h>

#define DAEMON_NAME "simpledaemon"

using namespace std;
using namespace rapidjson;

namespace tamageta {

enum action_type {
    SUBSCRIBE,
    UNSUBSCRIBE,
    MESSAGE
};

struct action {
    action(action_type t, connection_hdl h) : type(t), hdl(h) {}
    action(action_type t, connection_hdl h, server::message_ptr m)
      : type(t), hdl(h), msg(m) {}

    action_type type;
    websocketpp::connection_hdl hdl;
    server::message_ptr msg;
};

class Alexo : public vl53l0x_observer, public hcsr04_observer, public gy271_observer, public command_observer {
  public:
    Alexo();
    void init();
    void daemonize(string rundir, string pidfile);
    void run();
    void process_messages();
    void update(vl53l0x_event& event);
    void update(hcsr04_event& event);
    void update(gy271_event& event);
    void update(command_event& event);

  private:
    static void signal_handler(int sig);
    static void sigIntHndlr(int sig);
    static void sigIntExHndlr(int sig);
    static void daemonShutdown();

    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    void on_message(connection_hdl hdl, server::message_ptr msg);

    int force_exitx;
    static int force_exit;
    static int pidFilehandle;

    typedef std::set<connection_hdl,std::owner_less<connection_hdl> > con_list;

    server m_server;
    con_list m_connections;
    std::queue<action> m_actions;

    mutex m_action_lock;
    mutex m_connection_lock;
    condition_variable m_action_cond;

    //vl53l0x_worker vl53l0x{0};
    //hcsr04_worker hcsr04{28, 29};
    //gy271_worker gy271;
    pca9685 servo;
    mecanum_driver motorctrl{
      config::get_instance().get_front_right_wheel_1(),
      config::get_instance().get_front_right_wheel_2(),
      config::get_instance().get_front_left_wheel_1(),
      config::get_instance().get_front_left_wheel_2(),
      config::get_instance().get_rear_right_wheel_1(),
      config::get_instance().get_rear_right_wheel_2(),
      config::get_instance().get_rear_left_wheel_1(),
      config::get_instance().get_rear_left_wheel_2(),
      config::get_instance().get_front_right_sensor_1(),
      config::get_instance().get_front_right_sensor_2(),
      config::get_instance().get_front_left_sensor_1(),
      config::get_instance().get_front_left_sensor_2(),
      config::get_instance().get_rear_right_sensor_1(),
      config::get_instance().get_rear_right_sensor_2(),
      config::get_instance().get_rear_left_sensor_1(),
      config::get_instance().get_rear_left_sensor_2()
    };
    command_factory cmd_factory{servo, motorctrl};
};

int Alexo::force_exit = 0;
int Alexo::pidFilehandle;

Alexo::Alexo() {
  force_exitx = 0;

  wiringPiSetup(); // use wiringPi

  motorctrl.init_mode();
  servo.PwmSetup(0x40, 50);

  // Initialize Asio Transport
  m_server.init_asio();

  // Register handler callbacks
  m_server.set_open_handler(bind(&Alexo::on_open,this,::_1));
  m_server.set_close_handler(bind(&Alexo::on_close,this,::_1));
  m_server.set_message_handler(bind(&Alexo::on_message,this,::_1,::_2));
}

void Alexo::daemonShutdown(){
//  (pidFilehandle);
}

void Alexo::update(vl53l0x_event& event){
  websocketpp::lib::error_code ec;

  //std::cout << "received distance: " << event.getDistance() << std::endl;
  lock_guard<mutex> guard(m_connection_lock);

  std::string msg;
  message_handler::toJSON(event, msg);

  con_list::iterator it;
  for (it = m_connections.begin(); it != m_connections.end(); ++it) {
    m_server.send(*it,msg,websocketpp::frame::opcode::text, ec);
  }
}

void Alexo::update(hcsr04_event& event){
  websocketpp::lib::error_code ec;

  //std::cout << "received distance: " << event.getDistance() << std::endl;
  lock_guard<mutex> guard(m_connection_lock);

  std::string msg;
  message_handler::toJSON(event, msg);

  con_list::iterator it;
  for (it = m_connections.begin(); it != m_connections.end(); ++it) {
    m_server.send(*it,msg,websocketpp::frame::opcode::text, ec);
  }
}

void Alexo::update(gy271_event& event){
  websocketpp::lib::error_code ec;

  lock_guard<mutex> guard(m_connection_lock);

  std::string msg;
  message_handler::toJSON(event, msg);

  con_list::iterator it;
  for (it = m_connections.begin(); it != m_connections.end(); ++it) {
    m_server.send(*it,msg,websocketpp::frame::opcode::text, ec);
  }
}

void Alexo::update(command_event& event){
  websocketpp::lib::error_code ec;

  lock_guard<mutex> guard(m_connection_lock);

  std::string msg;
  if(typeid(event) == typeid(servo_command_event&)){
    message_handler::toJSON(static_cast<servo_command_event&>(event), msg);
  } else if(typeid(event) == typeid(servo_command_event&)){
    message_handler::toJSON(static_cast<drive_command_event&>(event), msg);
  }

  con_list::iterator it;
  for (it = m_connections.begin(); it != m_connections.end(); ++it) {
    m_server.send(*it,msg,websocketpp::frame::opcode::text, ec);
  }
}

void Alexo::signal_handler(int sig){
  switch(sig){
    case SIGHUP:
    case SIGINT:
    case SIGTERM:
      daemonShutdown();
      exit(EXIT_SUCCESS);
      break;
    default:
      break;
  }
}

void Alexo::daemonize(string rundir, string pidfile) {
  int pid, sid, i;
  char str[10];
  struct sigaction newSigAction;
  sigset_t newSigSet;

  if (getppid() == 1) {
    return;
  }

  sigemptyset(&newSigSet);
  sigaddset(&newSigSet, SIGCHLD);
  sigaddset(&newSigSet, SIGTSTP);
  sigaddset(&newSigSet, SIGTTOU);
  sigaddset(&newSigSet, SIGTTIN);
  sigprocmask(SIG_BLOCK, &newSigSet, NULL);

  newSigAction.sa_handler = signal_handler;
  sigemptyset(&newSigAction.sa_mask);
  newSigAction.sa_flags = 0;

  sigaction(SIGHUP, &newSigAction, NULL);
  sigaction(SIGTERM, &newSigAction, NULL);
  sigaction(SIGINT, &newSigAction, NULL);

  // fork returns child process id if process is parent or 0 if process is child process
  // this kills the parent process to be independent process and under init/kernel process
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  umask(027);

  sid = setsid();

  if (sid < 0) {
    exit(EXIT_FAILURE);
  }

  for (i = getdtablesize(); i >= 0; --i) {
    close(i);
  }

  i = open("/dev/null", O_RDWR);

  dup(i);
  dup(i);

  chdir(rundir.c_str());

  pidFilehandle = open(pidfile.c_str(), O_RDWR|O_CREAT, 0600);

  if (pidFilehandle == -1 ) {
//    BOOST_LOG_TRIVIAL(info) << "daemonize.pidFilehandle" << pidfile;
    exit(EXIT_FAILURE);
  }

  if (lockf(pidFilehandle,F_TLOCK,0) == -1) {
    exit(EXIT_FAILURE);
  }

  sprintf(str,"%d\n",getpid());

  write(pidFilehandle, str, strlen(str));

  close(pidFilehandle);
}

void Alexo::sigIntHndlr(int sig)
{
  force_exit = 1;
}

void Alexo::sigIntExHndlr(int sig)
{
  force_exit = 1;
  exit(EXIT_SUCCESS);
}

void Alexo::init(){
  int size = 0;
  
//  wiringPiSetupGpio();

//  size = config::get_instance().getI2CSize();
//  for(int i = 0; i < size; i++){
//    shared_ptr<I2CConf> p = config::get_instance().getI2CConf(i);
//    shared_ptr<PCA9685> pca = shared_ptr<PCA9685>(new PCA9685());
//    pca->Setup(p->address, p->hertz);
//    pca->PWMReset();
//    tamageta::app_ctx::get_instance().add(pca);
//  }
//
//  size = config::get_instance().getHcSr04ConfSize();
//  for(int i = 0; i < size; i++){
//    shared_ptr<HcSr04Conf> p = config::get_instance().getHcSr04Conf(i);
//    shared_ptr<HcSr04> hcsr04 = shared_ptr<HcSr04>(new HcSr04(p->pinTrig, p->pinEcho));
//    tamageta::app_ctx::get_instance().add(hcsr04);
//  }
}

void Alexo::run(){
  int opts = 0;
  int n = 0;
  unsigned int ms, oldms = 0;   

  cout << "Alexo::run() ... start\n" << endl;

  /* Deamonize */
  if(config::get_instance().is_daemon()){
    cout << "daemonize... " << config::get_instance().get_app_dir() << " : " << config::get_instance().get_pid_file() << endl;
    daemonize(config::get_instance().get_app_dir().c_str(), config::get_instance().get_pid_file().c_str());
    signal(SIGINT, sigIntHndlr);
    cout << "daemonized!" << endl;
  } else {
    signal(SIGINT, sigIntExHndlr);
    cout << "not daemonized!" << endl;
  }

//  init_log();

//  signal(SIGINT, sigIntHndlr);

  // listen on specified port
  m_server.listen(config::get_instance().get_port());

  cout << "Alexo::run() ... before accept\n" << endl;
  // Start the server accept loop
  m_server.start_accept();

  cout << "Alexo::run() ... before start vl53l0x\n" << endl;
  //vl53l0x.add((*this));
  //hcsr04.add((*this));
  //gy271.add((*this));

  //vl53l0x.start();
  //hcsr04.start();
  //gy271.start();

  // Start the ASIO io_service run loop
  cout << "Alexo::run() ... Start the ASIO io_service run loop\n" << endl;
  try {
    m_server.run();
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
  }
 
  cout << "Alexo::run() ... end\n" << endl;
}

void Alexo::on_open(connection_hdl hdl) {
  {
    lock_guard<mutex> guard(m_action_lock);
    m_actions.push(action(SUBSCRIBE,hdl));
  }
  m_action_cond.notify_one();
}

void Alexo::on_close(connection_hdl hdl) {
  {
    lock_guard<mutex> guard(m_action_lock);
    m_actions.push(action(UNSUBSCRIBE,hdl));
  }
  m_action_cond.notify_one();
}

void Alexo::on_message(connection_hdl hdl, server::message_ptr msg) {
  // queue message up for sending by processing thread
  {
    lock_guard<mutex> guard(m_action_lock);
    std::cout << "on_message" << std::endl;
    m_actions.push(action(MESSAGE,hdl,msg));
  }
  m_action_cond.notify_one();
}

void Alexo::process_messages() {
  while(1) {
    unique_lock<mutex> lock(m_action_lock);

    while(m_actions.empty()) {
      m_action_cond.wait(lock);
    }

    action a = m_actions.front();
    m_actions.pop();

    lock.unlock();

    if (a.type == SUBSCRIBE) {
      lock_guard<mutex> guard(m_connection_lock);
      m_connections.insert(a.hdl);
    } else if (a.type == UNSUBSCRIBE) {
      lock_guard<mutex> guard(m_connection_lock);
      m_connections.erase(a.hdl);
    } else if (a.type == MESSAGE) {
      lock_guard<mutex> guard(m_connection_lock);

      std::string cc{a.msg->get_payload()};

      std::cout << "MESSAGE: " << cc << std::endl;

      std::shared_ptr<command> mp = message_handler::toCommand(cmd_factory, cc);
      mp->add((*this));
      mp->doCommand();
    } else {
      // undefined.
    }
  }
}


} // end of namespace tamageta


int main(int argc, char **argv)
{
  cout << "Start main..." << endl;

  if ( argc != 2) {
    cerr << "argument count mismatch error." << endl;
    exit(EXIT_FAILURE);
  }
  tamageta::config::get_instance().load(argv[1]);

  cout << "AppDir:" << tamageta::config::get_instance().get_app_dir() << endl;
  cout << "LogDir:" << tamageta::config::get_instance().get_log_dir() << endl;
  cout << "PidFile:" << tamageta::config::get_instance().get_pid_file() << endl;

  tamageta::Alexo server;

  server.init();

  cout << "after init..." << endl;

  thread t(bind(&tamageta::Alexo::process_messages,&server));

  server.run();

  t.join();
 
  cout << "after server.run()..." << endl;

  return 0;
} //main

