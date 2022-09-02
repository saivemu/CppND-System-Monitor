#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor for Process class implemented with initializer list
Process::Process(int pid){
    pid_ = pid;
    user_ = LinuxParser::User(pid);
    command_ = LinuxParser::Command(pid);
    ram_ = LinuxParser::Ram(pid);
    uptime_ = LinuxParser::UpTime(pid);
    // Using the formula shared on https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    CpuUtilization_ = (LinuxParser::ActiveJiffies(pid)/sysconf(_SC_CLK_TCK))/uptime_;
}

// Return this process's ID
int Process::Pid() const { return this->pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return this->CpuUtilization_;}

// Return the command that generated this process
string Process::Command() const { return this->command_; }

// Return this process's memory utilization
string Process::Ram() const { return this->ram_; }

// Return the user (name) that generated this process
string Process::User() const { return this->user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return this->uptime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return CpuUtilization() < a.CpuUtilization(); }