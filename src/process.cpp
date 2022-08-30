#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor for Process class implemented with initializer list
Process::Process(int pid) : pid_(pid) {}

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
// Using the formula shared on https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() const { 
  float total_time = LinuxParser::ActiveJiffies(Process::Pid());
  float uptime_pid = LinuxParser::UpTime(Process::Pid());
  return (total_time/sysconf(_SC_CLK_TCK))/uptime_pid;
  }

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Process::Pid()); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Process::Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Process::Pid()); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return CpuUtilization() < a.CpuUtilization(); }