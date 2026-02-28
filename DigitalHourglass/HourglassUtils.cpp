#include "HourglassUtils.h"
#include <stdio.h>

// Define the global buffer
char timeString[6];

void formatTime(unsigned long seconds) {
    int m = seconds / 60;
    int s = seconds % 60;
    
    // snprintf is safer than sprintf because it prevents buffer overflows
    snprintf(timeString, sizeof(timeString), "%02d:%02d", m, s);
}