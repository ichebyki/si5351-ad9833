#ifndef UTILS_H
#define UTILS_H

//#define _SERIAL_LOG_

#ifdef _SERIAL_LOG_
    #define sp(a, b) \
        Serial.print(a); \
        Serial.println(b);
#else
    #define sp(a, b) (void *)0
#endif

#endif  // UTILS_H