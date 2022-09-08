#include <string>

#include "format.h"

using std::string;
using std::to_string;

#define hr_in_sec 3600
#define min_in_sec 60

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    long hours = seconds/hr_in_sec;
    seconds = (seconds%hr_in_sec);
    long minutes = (seconds/min_in_sec);
    seconds = seconds%min_in_sec;
    string seconds_str = to_string(seconds);
    string minutes_str = to_string(minutes);
    string hours_str = to_string(hours);
    seconds_str.insert(0, 2 - seconds_str.length(), '0');
    minutes_str.insert(0, 2 - minutes_str.length(), '0');
    hours_str.insert(0, 2 - hours_str.length(), '0');
    return hours_str + ":" + minutes_str+ ":" + seconds_str;
}