
#include "Settings.h"

Settings::Settings() {
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
    return(value);
}

void Settings::setWifiSSID(String value) {
    preferences.begin(SYSTEMNAME, RW_MODE);
    preferences.putString("wifiSSID", value.c_str());
    preferences.end();
}

String Settings::getWifiPassword(void) {
    preferences.begin(SYSTEMNAME, RO_MODE);
    String value = preferences.getString("wifiPassword", "12345678");
    preferences.end();
    return(value);
}

void Settings::setWifiPassword(String value) {
    preferences.begin(SYSTEMNAME, RW_MODE);
    preferences.putString("wifiPassword", value.c_str());
    preferences.end();
}

String Settings::getHttpUser(void) {
    preferences.begin(SYSTEMNAME, RO_MODE);
    String value = preferences.getString("httpUser", "admin");
    preferences.end();
    return(value);
}

void Settings::setHttpUser(String value) {
    preferences.begin(SYSTEMNAME, RW_MODE);
    preferences.putString("httpUser", value.c_str());
    preferences.end();
}

String Settings::getHttpPassword(void) {
    preferences.begin(SYSTEMNAME, RO_MODE);
    String value = preferences.getString("httpPassword", "admin");
    preferences.end();
    return(value);
}

void Settings::setHttpPassword(String value) {
    preferences.begin(SYSTEMNAME, RW_MODE);
    preferences.putString("httpPassword", value.c_str());
    preferences.end();
}

