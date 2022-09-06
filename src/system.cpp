#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>


#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    vector<Process> processes;
    // initializaing the processes vector with the number of processes in the pid_list
    vector<int> pid_list = LinuxParser::Pids();
    processes.reserve(pid_list.size());
    // loop through list of pids and create a process object for each pid
    for (long unsigned int i = 0; i < pid_list.size(); i++) {
        processes.emplace_back(CreateProcessObject(pid_list[i]));
    }
    // sort processes by cpu utilization as override operator defined in process class 
    // knowledge question answer explains the use of default sort operator https://knowledge.udacity.com/questions/805319
    std::sort(processes.rbegin(), processes.rend()); // using default < as sort operator
    processes_ = processes;
    return processes_;
}

Process System::CreateProcessObject(int pid){
    Process process(pid);
    process.CpuUtilization();
    
    return process;
}
// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
