
/*
 * Non-volatile system settings 
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>
#include "Finder.h"

#define SYSTEMNAME "datalogger"
#define RO_MODE true
#define RW_MODE false
#define MODBUS_SLOTS 5

typedef struct {
    uint16_t deviceAddress;
    uint8_t functionCode;
    uint16_t registerAddress;
    FinderType valueType;
} modbusConfig;

class Settings {
  public:
    Settings();

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
    bool getModbusConfig(uint8_t n, modbusConfig *config);
    bool setModbusConfig(uint8_t n, modbusConfig *config);

 private:
    Preferences preferences;
};

#endif
