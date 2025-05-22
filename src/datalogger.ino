
/*
 * modbus data logger 12.11.2024 JS
 */

#include <Arduino.h>
#include <ModbusMaster.h>
#include <RTClib.h>
#include <SD.h>
#include <Update.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "Timer_PFC8563.h"
#include "RESTful.h"
#include "Settings.h"

#define LED_GREEN GPIO_NUM_3
#define LED_YELLOW GPIO_NUM_4
#define BUTTON GPIO_NUM_5
#define TIMER GPIO_NUM_6
#define SERIAL1_TX 7
#define SERIAL1_RX 8
#define MAX485_DE 9
#define MAX485_RE_NEG 10

RTC_PCF8563 rtc;
Timer_PFC8563 timer;
ModbusMaster modbus;
AsyncWebServer *httpd;
RESTful restApi;
Settings settings;

RTC_DATA_ATTR bool enableWifi;
RTC_DATA_ATTR bool execJob;

bool fc4_T_float(uint16_t addr, float *value) {
    uint8_t result, a, b, c, d;
    uint16_t data[2];
    uint32_t i;

    result = modbus.readInputRegisters(addr, 2);
    if(result != modbus.ku8MBSuccess) return (false);
    for(i = 0; i < 2; i++) {
	data[i] = modbus.getResponseBuffer(i);
    }

    a = (data[0] >> 8) & 0xff;
    b = (data[0] >> 0) & 0xff;
    c = (data[1] >> 8) & 0xff;
    d = (data[1] >> 0) & 0xff;

    i = (a << 24) | (b << 16) | (c << 8) | (d << 0);
    memcpy(value, &i, 4);

    return (true);
}

bool fc4_T3(uint16_t addr, int32_t *value) {
    uint8_t result;
    uint16_t data[2];
    uint32_t i;

    result = modbus.readInputRegisters(addr, 2);
    if(result != modbus.ku8MBSuccess) return (false);
    for(i = 0; i < 2; i++) {
	data[i] = modbus.getResponseBuffer(i);
    }
    *value = (data[0] << 16) | data[1];

    return (true);
}

bool writeLogfile() {
    int n;
    bool ok;
    int32_t e1 = 0, e2 = 0, e3 = 0, e4 = 0;
    float watt = 0.0;
    char path[32];
    char message[64];

    // energy
    ok = fc4_T_float(2490, &watt);
    if(!ok) return (false);

    // mid counter
    ok = fc4_T3(462, &e1);
    if(!ok) return (false);
    ok = fc4_T3(464, &e2);
    if(!ok) return (false);
    ok = fc4_T3(466, &e3);
    if(!ok) return (false);
    ok = fc4_T3(468, &e4);
    if(!ok) return (false);

    // rtc
    DateTime now = rtc.now();

    // directory
    sprintf(path, "/%4d", now.year());
    if(!SD.exists(path)) {
	SD.mkdir(path);
    }

    // file
    n = snprintf(path, sizeof(path), "/%4.4d/%2.2d%2.2d", now.year(),
		 now.month(), now.day());
    if(n < 0) return (false);
    if(File file = SD.open(path, FILE_APPEND)) {
	n = snprintf(message, sizeof(message),
		     "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d %d %d %d %d %.1f\n",
		     now.year(), now.month(), now.day(), now.hour(),
		     now.minute(), now.second(), e1, e2, e3, e4, watt);
	if(n < 0) return (false);
	file.print(message);
	file.close();
    } else return (false);

    return (true);
}

void preTransmission() {
    digitalWrite(MAX485_RE_NEG, HIGH);
    digitalWrite(MAX485_DE, HIGH);
}

void postTransmission() {
    digitalWrite(MAX485_RE_NEG, LOW);
    digitalWrite(MAX485_DE, LOW);
}

void progressCallBack(size_t currSize, size_t totalSize) {
    digitalWrite(LED_YELLOW, !digitalRead(LED_YELLOW));
}

void IRAM_ATTR isrButton() {
    if(millis() > 500) {
	enableWifi = false;
    }
}

void IRAM_ATTR isrTimer() {
    execJob = true;
}

void setup() {
    uint64_t bitmask;

    // gpio
    pinMode(LED_GREEN, OUTPUT);
    digitalWrite(LED_GREEN, LOW);
    pinMode(LED_YELLOW, OUTPUT);
    digitalWrite(LED_YELLOW, LOW);
    pinMode(MAX485_RE_NEG, OUTPUT);
    pinMode(MAX485_DE, OUTPUT);
    digitalWrite(MAX485_RE_NEG, LOW);
    digitalWrite(MAX485_DE, LOW);
    pinMode(BUTTON, INPUT);
    attachInterrupt(BUTTON, isrButton, FALLING);
    pinMode(TIMER, INPUT);
    attachInterrupt(TIMER, isrTimer, FALLING);

    // modbus
    Serial1.begin(19200, SERIAL_8N2, SERIAL1_RX, SERIAL1_TX);
    modbus.begin(33, Serial1);
    modbus.preTransmission(preTransmission);
    modbus.postTransmission(postTransmission);

    // rtc
    rtc.begin();
    if(rtc.lostPower()) {
	// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    // timer
    if(!timer.isEnabled()) {
	timer.enable(1);
    }

    // sd card
    SD.begin();

    // firmware
    if(SD.exists("/firmware.bin")) {
        File firmware = SD.open("/firmware.bin");
        if(firmware) {
	    Update.onProgress(progressCallBack);
	    Update.begin(firmware.size(), U_FLASH);
	    Update.writeStream(firmware);
	    firmware.close();
        }
        SD.remove("/firmware.bin");
	if(Update.end()) {
	    digitalWrite(LED_YELLOW, LOW);
	    delay(500);
	    ESP.restart();
        }
    }

    // wakeup
    switch (esp_sleep_get_wakeup_cause()) {
     case ESP_SLEEP_WAKEUP_EXT1:
	 bitmask = esp_sleep_get_ext1_wakeup_status();
	 if(bitmask & (1ULL << BUTTON)) enableWifi = true;
	 if(bitmask & (1ULL << TIMER)) execJob = true;
	 break;
     default:
	 execJob = false;
	 enableWifi = false;

	 // boot blink
	 digitalWrite(LED_GREEN, HIGH);
	 digitalWrite(LED_YELLOW, HIGH);
	 delay(500);
	 digitalWrite(LED_GREEN, LOW);
	 digitalWrite(LED_YELLOW, LOW);
	 break;
    }

    // preferences
    settings.begin();

    if(enableWifi) {
	// led
	digitalWrite(LED_YELLOW, HIGH);

	// start Wifi
	WiFi.softAP(settings.wifiSSID.c_str(), settings.wifiPassword.c_str());
	IPAddress IP = IPAddress(10, 0, 0, 1);
	IPAddress Netmask = IPAddress(255, 255, 255, 0);
	WiFi.softAPConfig(IP, IP, Netmask);

        // mdns
        MDNS.begin(NAMESPACE);

        // http
        httpd = new AsyncWebServer(80);
        httpd->on("^\\/api\\/rtc\\/date$",
                    std::bind(&RESTful::rtcDate, restApi, std::placeholders::_1));
        httpd->on("^\\/api\\/logs$", HTTP_GET,
                    std::bind(&RESTful::logsList, restApi, std::placeholders::_1));
        httpd->on("^\\/api\\/logs\\/([0-9][0-9][0-9][0-9])([0-9][0-9][0-9][0-9])$", HTTP_GET,
                    std::bind(&RESTful::logsFile, restApi, std::placeholders::_1));
        httpd->on("^\\/api\\/firmware\\/upload$", HTTP_POST,
                    std::bind(&RESTful::firmwareUpload, restApi, std::placeholders::_1),
                    std::bind(&RESTful::firmwareUploadChunks, restApi, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                    std::placeholders::_5, std::placeholders::_6));
        httpd->onNotFound([](AsyncWebServerRequest * request) {
            request->send(404, "text/plain", "Not found");
        });
        httpd->begin();
    }
}

void loop() {
    uint64_t bitmask;

    if(execJob) {
	execJob = false;
	if(writeLogfile()) {
	    digitalWrite(LED_GREEN, HIGH);
	    delay(500);
	    digitalWrite(LED_GREEN, LOW);
	}
    }

    if(!enableWifi) {
	// stop Wifi
	WiFi.softAPdisconnect(true);

	// debounce button
	delay(500);

	// sleep
	bitmask = (1ULL << BUTTON) | (1ULL << TIMER);
	esp_sleep_enable_ext1_wakeup(bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
	esp_deep_sleep_start();
    }
}
