#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "linux_parser.h"

using std::string;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();            
  string User();              
  string Command();               
  float CpuUtilization();                 
  string Ram();              
  long int UpTime();                  
  bool operator<(Process const& a) const;

 private:
  int pid_;
  string user_;
  string command_;
  float CpuUtilization_;
};

#endif