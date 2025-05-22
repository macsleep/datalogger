
#include "Settings.h"

Settings::Settings() {
}

void Settings::begin(void) {
	char buffer[128] = "";
	uint64_t mac = ESP.getEfuseMac();
	sprintf(buffer, "Datalogger_%02X%02X%02X", uint8_t((mac >> 24) & 0xff), uint8_t((mac >> 32) & 0xff), uint8_t((mac >> 40) & 0xff));
        String ssid = String(buffer);

	preferences.begin(NAMESPACE, RO_MODE);
	this->wifiSSID = preferences.getString("wifiSSID", ssid);
	this->wifiPassword = preferences.getString("wifiPassword", "12345678");
	this->httpUser = preferences.getString("httpUser", "admin");
	this->httpPassword = preferences.getString("httpPassword", "admin");
	preferences.end();
}

void Settings::save(void) {
	preferences.begin(NAMESPACE, RW_MODE);
	preferences.putString("wifiSSID", this->wifiSSID);
	preferences.putString("wifiPassword", this->wifiPassword);
	preferences.putString("httpUser", this->httpUser);
	preferences.putString("httpPassword", this->httpPassword);
	preferences.end();
}

void Settings::clear(void) {
	preferences.begin(NAMESPACE, RW_MODE);
	preferences.clear();
	preferences.end();
}

