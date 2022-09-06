#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

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
  string token;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> token >> value) {
        if (token == "PRETTY_NAME") {
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
  string line, MemTotalStr, MemFreeStr;
  float memory_utilization = 0.0, mem_total = 0.0, mem_free = 0.0;
  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    while(stream >> line){
      if(line == "MemTotal:"){
        stream >> MemTotalStr;
        if (MemTotalStr != ""){
          mem_total = stof(MemTotalStr);
        }
      }
      if(line == "MemFree:"){
        stream >> MemFreeStr;
        if (MemFreeStr != ""){
          mem_free = stof(MemFreeStr);
        }
        break;
      }  
    }
    if (mem_total != 0.0){
      memory_utilization = (mem_total - mem_free) / mem_total;
    }
  }
  return memory_utilization;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, uptime_str;
  long uptime = 0;
  ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    linestream >> uptime_str;
    uptime = stol(uptime_str);
    stream.close();
  }
   return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  // based on answer from https://knowledge.udacity.com/questions/129844
  return UpTime() * sysconf(_SC_CLK_TCK); 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  long utime = 0, stime = 0, cutime = 0, cstime = 0;
  if(stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    string temp;
    string utime_str, stime_str, cutime_str, cstime_str;
    // skip the first 13 values
    for(int i=0; i<13; i++){
      linestream >> temp;
    }
    // get the next 4 values and add them for the total time
    linestream >> utime_str >> stime_str >> cutime_str >> cstime_str;
    utime = stol(utime_str);
    stime = stol(stime_str);
    cutime = stol(cutime_str);
    cstime = stol(cstime_str);
    
    stream.close();
  }
  return utime + stime + cutime + cstime; // return 0 if the stream is not open
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();
  // as described in the link https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // use enums to access the vector
  long userJiffies = stol(jiffies[CPUStates::kUser_]);
  long niceJiffies = stol(jiffies[CPUStates::kNice_]);
  long systemJiffies = stol(jiffies[CPUStates::kSystem_]);
  long irqJiffies = stol(jiffies[CPUStates::kIRQ_]);
  long softirqJiffies = stol(jiffies[CPUStates::kSoftIRQ_]);
  long stealJiffies = stol(jiffies[CPUStates::kSteal_]);

  long active_jiffies = userJiffies + niceJiffies + systemJiffies + irqJiffies + softirqJiffies + stealJiffies; 

  return active_jiffies; 
  }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  // as described in https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // answer also based on https://knowledge.udacity.com/questions/129844
  long idleJiffies = stol(jiffies[CPUStates::kIdle_]);
  long iowaitJiffies = stol(jiffies[CPUStates::kIOwait_]);

  return idleJiffies+iowaitJiffies;
  }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_utilization_times {};
  string line, token, cpu_states;
  ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    linestream >> token;
    if(token == "cpu"){
      while(linestream >> cpu_states){
        cpu_utilization_times.push_back(cpu_states);
      }
    }
    stream.close();
  }
  return cpu_utilization_times;;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  int total_processes = 0;
  ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
  while(stream >> line){
    if(line == "processes"){
      stream >> total_processes;
      break;
    }
  }
    stream.close();
  }
  return total_processes;
  }

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  int running_processes = 0;
  ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(stream >> line){
      if(line == "procs_running"){
        stream >> running_processes;
        break;
      }
    }
    stream.close();
  }
  return running_processes; 
  }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string command;
  ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);  
  if(stream.is_open()){
    getline(stream, command);
    stream.close();
  }
  return command; 
  }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string mem_pid;
  int ram = 0;
  string token;
  ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  if (stream.is_open()){
    while (stream >> token){
      if(token == "VmSize:"){
        stream >> mem_pid;
        if (mem_pid!=""){
          ram = stoi(mem_pid)/1024;
        }
        break;
      }
    }
    stream.close();
  }
  return to_string(ram);
  }

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string uid;
  string token;
  ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  if (stream.is_open()){
    while (stream >> token){
      if(token == "Uid:"){
        stream >> uid;
        break;
      }
    }
    stream.close();
  }
  return uid; 
  }

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string user_id = Uid(pid);
  string line, user, x, uid;
  ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while(getline(stream, line)){
      // replace : with space for easier parsing
      std::replace(line.begin(), line.end(), ':', ' ');
      istringstream linestream(line);
      // get the user, x and uid tokens from the line
      linestream >> user;
      linestream >> x;
      linestream >> uid;
      if(uid == user_id){
        return user;
      }
    }
    stream.close();
  }
  return "UNKNOWN"; // return default user if the stream is not open
  }

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line;
  // define default value to avoid compiler error during uptime calculation outside the loop
  long starttime = 0;
  ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if (stream.is_open()){
    getline(stream, line);
    istringstream linestream(line);
    string value;
    // skip the first 21 values
    // based on formula stated in the course
    for(int i=0; i<21; i++){
      linestream >> value;
    }
    linestream >> value;
    starttime = stol(value);
    stream.close();
  }
  long uptime_pid = UpTime() - (starttime/sysconf(_SC_CLK_TCK));
  return uptime_pid;
  }
