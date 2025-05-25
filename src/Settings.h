
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

 private:
    Preferences preferences;
};

#endif
