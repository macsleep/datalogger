/*
  Copyright 2025 Jan Schlieper

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
 */

#ifndef TIMER_PFC8563_H
#define TIMER_PFC8563_H

#include <Arduino.h>
#include <Wire.h>

#define PCF8563_ADDRESS 0x51
#define PCF8563_CONTROL_STATUS_1 0x00
#define PCF8563_CONTROL_STATUS_2 0x01
#define PCF8563_CLKOUT_CONTROL 0x0d
#define PCF8563_TIMER_CONTROL 0x0e
#define PCF8563_TIMER 0x0f

class Timer_PFC8563 {
    public:
        Timer_PFC8563();
        void enable(uint8_t value);
        bool isEnabled();
        void disable();

    private:
        void clkoutEnable();
        void clkoutDisable();
};

#endif
