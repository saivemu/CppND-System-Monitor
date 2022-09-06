#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor for Process class
Process::Process(int pid){
    pid_ = pid;
    user_ = LinuxParser::User(pid);
    command_ = LinuxParser::Command(pid);
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    // Using the formula shared on https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599   
    long int uptime_ = Process::UpTime();
    
    if (uptime_ == 0){
        uptime_ = sysconf(_SC_CLK_TCK); // to avoid division by zero on first instance of process start
    }
    // Implement this outside of the constructor to enable updating of the values
    CpuUtilization_ = (((float)LinuxParser::ActiveJiffies(pid_))/(float)sysconf(_SC_CLK_TCK))/(float)uptime_;
    return CpuUtilization_;
}

// Return the command that generated this process
string Process::Command() { return command_; }

// Return this process's memory utilization
string Process::Ram() { 
    string ram_ = LinuxParser::Ram(pid_);
    return ram_;
}

// Return the user (name) that generated this process
string Process::User() { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    long int uptime_ = LinuxParser::UpTime(pid_);
    return uptime_; 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return this->CpuUtilization_ < a.CpuUtilization_; }