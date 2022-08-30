#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stol;
using std::ifstream;
using std::istringstream;
using std::getline;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, MemTotal, MemFree;
  float memory_utilization;
  ifstream stream(kProcDirectory + kMeminfoFilename);
  while(stream >> line){
    if(line == "MemTotal:"){
      stream >> MemTotal;
    }
    if(line == "MemFree:"){
      stream >> MemFree;
      break;
    }
  }
  memory_utilization = (stof(MemTotal) - stof(MemFree))/stof(MemTotal);
  return memory_utilization;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, uptime_str;
  long uptime;
  ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    linestream >> uptime_str;
    uptime = stol(uptime_str);
  }
  return uptime; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return ActiveJiffies() + IdleJiffies(); 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    string value;
    // skip the first 13 values
    for(int i=0; i<13; i++){
      linestream >> value;
    }
    // get the next 4 values and add them for the total time
    linestream >> value;
    long utime = stol(value);
    linestream >> value;
    long stime = stol(value);
    linestream >> value;
    long cutime = stol(value);
    linestream >> value;
    long cstime = stol(value);
    return utime + stime + cutime + cstime;
  }
  return 0; // return 0 if the stream is not open
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();
  long active_jiffies = stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) + stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) + stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);
  return active_jiffies; 
  }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  long idle_jiffies = stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
  return idle_jiffies; 
  }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_utilization_times; 
  string line, key, cpu_states;
  ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    linestream >> key;
    if(key == "cpu"){
      while(linestream >> cpu_states){
        cpu_utilization_times.push_back(cpu_states);
      }
      return cpu_utilization_times;
    }
  }
  return {}; 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  int total_processes;
  ifstream stream(kProcDirectory + kStatFilename);
  while(stream >> line){
    if(line == "processes"){
      stream >> total_processes;
      break;
    }
  }
  return total_processes;
  }

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  int running_processes;
  ifstream stream(kProcDirectory + kStatFilename);
  while(stream >> line){
    if(line == "procs_running"){
      stream >> running_processes;
      break;
    }
  }
  return running_processes; 
  }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string command;
  ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);  
  if(stream.is_open()){
    getline(stream, command);
  }
  return command; 
  }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string mem_pid;
  string key;
  ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  while (stream >> key){
    if(key == "VmSize:"){
      stream >> mem_pid;
      break;
    }
  }
  return to_string(stoi(mem_pid)/1000); 
  }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string uid_pid;
  string key;
  ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  while(stream >> key){
    if(key == "Uid:"){
      stream >> uid_pid;
      break;
    }
  }
  return uid_pid; 
  }

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid_this_pid = Uid(pid);
  string line, user, x, uid;
  ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while(getline(stream, line)){
      // replace : with space for easier parsing
      std::replace(line.begin(), line.end(), ':', ' ');
      istringstream linestream(line);
      linestream >> user >> x >> uid;
      if(uid == uid_this_pid){
        return user;
      }
    }
  }
  return "DEFAULT_USER"; 
  }

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line;
  ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if (stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    string value;
    // skip the first 20 values
    for(int i=0; i<21; i++){
      linestream >> value;
    }
    linestream >> value;
    long starttime = stol(value);
  }
  long uptime_this_pid = UpTime() - (starttime/sysconf(_SC_CLK_TCK));
  return uptime_this_pid;
  }
