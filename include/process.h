#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using std::string;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid() const;                               
  string User() const;                      
  string Command() const;                   
  float CpuUtilization() const;                  
  string Ram() const;                       
  long int UpTime() const;                       
  bool operator<(Process const& a) const;  

 private:
  int pid_;
  string user_;
  string command_;
  float CpuUtilization_;
  string ram_;
  long uptime_;
};

#endif