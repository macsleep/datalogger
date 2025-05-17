
/*
 Real Time Clock PCF8563 Timer functions
*/

#ifndef TIMER_PFC8563_H
#define TIMER_PFC8563_H

#include <Arduino.h>
#include <Wire.h>

#define PCF8563_ADDRESS 0x51
#define PCF8563_CONTROL_STATUS_1 0x00
#define PCF8563_CONTROL_STATUS_2 0x01
#define PCF8563_TIMER_CONTROL 0x0e
#define PCF8563_TIMER 0x0f

class Timer_PFC8563 {
    public:
        Timer_PFC8563();
        void enable(uint8_t value);
        bool isEnabled();
        void disable();
    private:
};

#endif
