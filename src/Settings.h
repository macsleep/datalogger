
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
    // methods
    Settings();
    void begin(void);
    void save(void);
    void clear(void);

    // variables
    String wifiSSID;
    String wifiPassword;
    String httpUser;
    String httpPassword;

  private:
    // variables
     Preferences preferences;
};

#endif
