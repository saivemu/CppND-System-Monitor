#include "processor.h"
#include "linux_parser.h"


// Return the aggregate CPU utilization
float Processor::Utilization() { 
    // using formula from https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    // using udacity knowledge question answer https://knowledge.udacity.com/questions/543543 for initializing prev values to 0
    long idle = LinuxParser::IdleJiffies();
    long active = LinuxParser::ActiveJiffies();
    long total = idle + active;
    long deltaIdle = idle - prevIdle;
    long deltaTotal = total - prevTotal;
    prevIdle = idle;
    prevTotal = total;
    return (float)(deltaTotal - deltaIdle) / deltaTotal;
}