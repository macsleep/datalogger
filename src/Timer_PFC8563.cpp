
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
