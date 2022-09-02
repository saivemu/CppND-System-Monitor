#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {     
    string formatted_time = to_string(seconds/3600) + ":" + to_string(seconds%3600/60) + ":" + to_string(seconds%60);
    return formatted_time; 
}