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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>
#include <nvs_flash.h>
#include "Finder.h"

#define SYSTEMNAME "datalogger"
#define RO_MODE true
#define RW_MODE false
#define MODBUS_SLOTS 10

typedef struct {
    uint8_t deviceAddress;
    uint8_t functionCode;
    uint16_t registerAddress;
    FinderType valueType;
} ModbusConfig;

class Settings {
  public:
    Settings();

    void reset(void);
    String getWifiSSID(void);
    void setWifiSSID(String value);
    String getWifiPassword(void);
    void setWifiPassword(String value);
    String getHttpUser(void);
    void setHttpUser(String value);
    String getHttpPassword(void);
    void setHttpPassword(String value);
    uint8_t getTimer(void);
    void setTimer(uint8_t value);
    bool getModbusConfig(uint8_t n, ModbusConfig * config);
    bool setModbusConfig(uint8_t n, ModbusConfig * config);
    unsigned long getSerial1Baud(void);
    void setSerial1Baud(unsigned long value);
    uint32_t getSerial1Config(void);
    void setSerial1Config(uint32_t value);

  private:
     Preferences preferences;
};

#endif
