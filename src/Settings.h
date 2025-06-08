
/*
 * Non-volatile system settings 
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>

#define SYSTEMNAME "datalogger"
#define RO_MODE true
#define RW_MODE false

enum class FinderType { T1, T3, T_float };
 
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

 private:
    const char* FinderTypeToString(FinderType e) {
        switch(e) {
            case FinderType::T1: return "T1";
            case FinderType::T3: return "T3";
            case FinderType::T_float: return "T_float";
            default: return "";
        }
    }

    Preferences preferences;
};

#endif
