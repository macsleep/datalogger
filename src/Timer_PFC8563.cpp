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

#include "Timer_PFC8563.h"

Timer_PFC8563::Timer_PFC8563() {
}

void Timer_PFC8563::enable(uint8_t value) {
    uint8_t data;

    Wire.beginTransmission(PCF8563_ADDRESS);
    Wire.write(PCF8563_TIMER_CONTROL);
    bitSet(data, 0);		// TD 1/60 Hz
    bitSet(data, 1);		// TD 1/60 Hz
    bitSet(data, 7);		// TE
    Wire.write(B10000011);	// enable 1/60 Hz clock
    Wire.endTransmission();

    Wire.beginTransmission(PCF8563_ADDRESS);
    Wire.write(PCF8563_TIMER);
    Wire.write(value);		// timer value
    Wire.endTransmission();

    Wire.beginTransmission(PCF8563_ADDRESS);
    Wire.write(PCF8563_CONTROL_STATUS_2);
    Wire.endTransmission();

    // Control_status_2
    Wire.requestFrom(PCF8563_ADDRESS, 1);
    data = Wire.read();
    bitSet(data, 0);		// TIE
    bitSet(data, 4);		// TI_TP

    Wire.beginTransmission(PCF8563_ADDRESS);
    Wire.write(PCF8563_CONTROL_STATUS_2);
    Wire.write(data);
    Wire.endTransmission();
}

bool Timer_PFC8563::isEnabled() {
    uint8_t data;
    bool flag;

    Wire.beginTransmission(PCF8563_ADDRESS);
    Wire.write(PCF8563_TIMER_CONTROL);
    Wire.endTransmission();

    // Timer_control
    Wire.requestFrom(PCF8563_ADDRESS, 1);
    data = Wire.read();
    flag = bitRead(data, 7);	// TE 

    return (flag);
}

void Timer_PFC8563::disable(void) {
    uint8_t data;

    Wire.beginTransmission(PCF8563_ADDRESS);
    Wire.write(PCF8563_TIMER_CONTROL);
    Wire.endTransmission();

    // Timer_control
    Wire.requestFrom(PCF8563_ADDRESS, 1);
    data = Wire.read();
    bitClear(data, 7);		// TE

    Wire.beginTransmission(PCF8563_ADDRESS);
    Wire.write(PCF8563_TIMER_CONTROL);
    Wire.write(data);
    Wire.endTransmission();
}
