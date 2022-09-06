#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
    // using udacity knowledge question answer https://knowledge.udacity.com/questions/543543 for initializing prev values to 0
    // declare values that need persisting between calls
    long prevIdle = 0;
    long prevActive = 0;
    long prevTotal = 0;
};

#endif