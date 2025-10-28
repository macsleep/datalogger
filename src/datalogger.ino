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

#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "Timer_PFC8563.h"
#include "rest/API.h"
#include "Settings.h"
#include "Finder.h"
#include "Utils.h"

#define LED_GREEN GPIO_NUM_4
#define LED_YELLOW GPIO_NUM_5
#define BUTTON GPIO_NUM_6
#define TIMER GPIO_NUM_7
#define SERIAL1_TX GPIO_NUM_17
#define SERIAL1_RX GPIO_NUM_18
#define RS485_DE_RE GPIO_NUM_19

RTC_PCF8563 rtc;
Timer_PFC8563 timer;
AsyncWebServer *httpd;
ModbusMaster modbus;
Finder energyMeter;
REST::API restAPI;
Settings settings;
Utils utils;

RTC_DATA_ATTR bool enableWifi;
RTC_DATA_ATTR bool execJob;

bool writeLogfile() {
    int n, i;
    char path[64];
    char buffer[64];
    String line = "";
    ModbusConfig config;

    // rtc
    DateTime now = rtc.now();

    // year directory
    snprintf(path, sizeof(path), "/%04d", now.year());
    if(!SD.exists(path)) SD.mkdir(path);

    // month directory
    snprintf(path, sizeof(path), "/%04d/%02d", now.year(), now.month());
    if(!SD.exists(path)) SD.mkdir(path);

    // file
    n = snprintf(path, sizeof(path), "/%04d/%02d/%02d", now.year(), now.month(), now.day());
    if(n < 0) return (false);
    if(File file = SD.open(path, FILE_APPEND)) {

        // timestamp
        n = snprintf(buffer, sizeof(buffer), "%04d%02d%02d%02d%02d%02d",
                     now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
        if(n < 0) return (false);
        line += String(buffer);

        // modbus
        i = 0;
        while(settings.getModbusConfig(i++, &config)) {
            if(config.deviceAddress == 0 || config.valueType == FinderType::FOO) continue;
            String value = energyMeter.getModbus(config.deviceAddress, config.functionCode,
                                                 config.registerAddress, config.valueType);
            if(value.length() > 0) line += " " + value;
        }

        // write log
        file.println(line.c_str());
        file.close();
    } else return (false);

    return (true);
}

void preTransmission() {
    digitalWrite(RS485_DE_RE, HIGH);
}

void postTransmission() {
    digitalWrite(RS485_DE_RE, LOW);
}

void IRAM_ATTR isrButton() {
    // debounce button
    if(millis() > 500) {
        enableWifi = false;
    }
}

void IRAM_ATTR isrTimer() {
    execJob = true;
}

void setup() {
    bool ok = true;
    uint64_t bitmask;
    timeval tv;

    // gpio
    pinMode(LED_GREEN, OUTPUT);
    digitalWrite(LED_GREEN, LOW);
    pinMode(LED_YELLOW, OUTPUT);
    digitalWrite(LED_YELLOW, LOW);
    pinMode(RS485_DE_RE, OUTPUT);
    digitalWrite(RS485_DE_RE, LOW);
    pinMode(BUTTON, INPUT);
    attachInterrupt(BUTTON, isrButton, FALLING);
    pinMode(TIMER, INPUT);
    attachInterrupt(TIMER, isrTimer, FALLING);

    // modbus
    Serial1.begin(settings.getSerial1Baud(), settings.getSerial1Config(), SERIAL1_RX, SERIAL1_TX);
    modbus.preTransmission(preTransmission);
    modbus.postTransmission(postTransmission);
    energyMeter.begin(&Serial1, &modbus);

    // peripheral
    ok &= rtc.begin();
    ok &= SD.begin();
    if(!ok) {
        // sleep
        bitmask = (1ULL << BUTTON);
        esp_sleep_enable_ext1_wakeup(bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
        esp_deep_sleep_start();
    }

    // timer
    if(!timer.isEnabled()) {
        timer.enable(settings.getTimer());
    }

    // system time
    tv.tv_sec = rtc.now().unixtime();
    settimeofday(&tv, NULL);

    // wakeup
    switch(esp_sleep_get_wakeup_cause()) {
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

    if(enableWifi) {
        // led
        digitalWrite(LED_YELLOW, HIGH);

        // start Wifi
        WiFi.softAP(settings.getWifiSSID().c_str(), settings.getWifiPassword().c_str());
        IPAddress IP = IPAddress(10, 0, 0, 1);
        IPAddress Netmask = IPAddress(255, 255, 255, 0);
        WiFi.softAPConfig(IP, IP, Netmask);

        // mdns
        MDNS.begin(SYSTEMNAME);

        // http
        httpd = new AsyncWebServer(80);
        restAPI.begin(httpd);
        if(SPIFFS.begin()) {
            httpd->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
        }
        httpd->onNotFound([](AsyncWebServerRequest * request) {
            request->send(404, "text/plain", "Not found");
        });
        httpd->begin();
    }
}

void loop() {
    uint64_t bitmask;

    if(utils.updateAvailable()) {
        utils.update(LED_YELLOW);
    }

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
        WiFi.mode(WIFI_OFF);

        // debounce button
        delay(500);

        // sleep
        bitmask = (1ULL << BUTTON) | (1ULL << TIMER);
        esp_sleep_enable_ext1_wakeup(bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
        esp_deep_sleep_start();
    }
}
