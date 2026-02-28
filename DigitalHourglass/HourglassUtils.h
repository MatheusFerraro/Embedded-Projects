#ifndef HOURGLASS_UTILS_H
#define HOURGLASS_UTILS_H

#include <Arduino.h>

extern char timeString[6];

// Function prototype for the utility function
void formatTime(unsigned long seconds);

#endif