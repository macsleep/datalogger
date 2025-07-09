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

#include "Settings.h"

Settings::Settings() {
}

void Settings::reset(void) {
    int i;
    ModbusConfig config;

    nvs_flash_erase();
    nvs_flash_init();

    delay(500);

    setWifiSSID(getWifiSSID());
    setWifiPassword(getWifiPassword());
    setHttpUser(getHttpUser());
    setHttpPassword(getHttpPassword());
    setTimer(getTimer());

    for(i = 0; i < MODBUS_SLOTS; i++) {
	getModbusConfig(i, &config);
	setModbusConfig(i, &config);
    }
}

String Settings::getWifiSSID(void) {
    char buffer[128] = "";
    uint64_t mac = ESP.getEfuseMac();
    sprintf(buffer, "Datalogger_%02X%02X%02X", uint8_t((mac >> 24) & 0xff),
	    uint8_t((mac >> 32) & 0xff), uint8_t((mac >> 40) & 0xff));
    String ssid = String(buffer);

    preferences.begin(SYSTEMNAME, RO_MODE);
    String value = preferences.getString("wifiSSID", ssid);
    preferences.end();
    return (value);
}

void Settings::setWifiSSID(String value) {
    if(preferences.begin(SYSTEMNAME, RW_MODE)) {
        preferences.putString("wifiSSID", value.c_str());
        preferences.end();
    }
}

String Settings::getWifiPassword(void) {
    preferences.begin(SYSTEMNAME, RO_MODE);
    String value = preferences.getString("wifiPassword", "12345678");
    preferences.end();
    return (value);
}

void Settings::setWifiPassword(String value) {
    if(preferences.begin(SYSTEMNAME, RW_MODE)) {
        preferences.putString("wifiPassword", value.c_str());
        preferences.end();
    }
}

String Settings::getHttpUser(void) {
    preferences.begin(SYSTEMNAME, RO_MODE);
    String value = preferences.getString("httpUser", "admin");
    preferences.end();
    return (value);
}

void Settings::setHttpUser(String value) {
    if(preferences.begin(SYSTEMNAME, RW_MODE)) {
        preferences.putString("httpUser", value.c_str());
        preferences.end();
    }
}

String Settings::getHttpPassword(void) {
    preferences.begin(SYSTEMNAME, RO_MODE);
    String value = preferences.getString("httpPassword", "admin");
    preferences.end();
    return (value);
}

void Settings::setHttpPassword(String value) {
    if(preferences.begin(SYSTEMNAME, RW_MODE)) {
        preferences.putString("httpPassword", value.c_str());
        preferences.end();
    }
}

uint8_t Settings::getTimer(void) {
    preferences.begin(SYSTEMNAME, RO_MODE);
    uint8_t value = preferences.getUChar("timer", 0);
    preferences.end();
    return (value);
}

void Settings::setTimer(uint8_t value) {
    if(preferences.begin(SYSTEMNAME, RW_MODE)) {
        preferences.putUChar("timer", value);
        preferences.end();
    }
}

bool Settings::getModbusConfig(uint8_t n, ModbusConfig *config) {
    char key[16];

    if(n >= MODBUS_SLOTS) return (false);

    preferences.begin(SYSTEMNAME, RO_MODE);
    snprintf(key, sizeof(key), "modbus_%d", n);
    if(preferences.isKey(key) && preferences.getBytesLength(key) == sizeof(ModbusConfig)) {
	preferences.getBytes(key, config, sizeof(ModbusConfig));
    } else bzero(config, sizeof(ModbusConfig));
    preferences.end();

    return (true);
}

bool Settings::setModbusConfig(uint8_t n, ModbusConfig *config) {
    char key[16];

    if(n >= MODBUS_SLOTS) return (false);

    if(preferences.begin(SYSTEMNAME, RW_MODE)) {
        snprintf(key, sizeof(key), "modbus_%d", n);
        preferences.putBytes(key, config, sizeof(ModbusConfig));
        preferences.end();
    }

    return (true);
}
