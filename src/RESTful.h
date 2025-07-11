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

#ifndef RESTFUL_H
#define RESTFUL_H

#include <Arduino.h>
#include <SD.h>
#include <RTClib.h>
#include <Regexp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HardwareSerial.h>
#include "Timer_PFC8563.h"
#include "Settings.h"
#include "Utils.h"

extern RTC_PCF8563 rtc;
extern Timer_PFC8563 timer;
extern Finder energyMeter;;
extern Settings settings;

class RESTful {
  public:
    RESTful();
    void begin(AsyncWebServer *httpd);
    void rtcConfig(AsyncWebServerRequest *request);
    void timerConfig(AsyncWebServerRequest *request);
    void logsList(AsyncWebServerRequest *request);
    void logsFile(AsyncWebServerRequest *request);
    void logsFileChunks(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void logsRemove(AsyncWebServerRequest *request);
    void firmwareVersion(AsyncWebServerRequest *request);
    void firmwareUpload(AsyncWebServerRequest *request);
    void firmwareUploadChunks(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void systemConfig(AsyncWebServerRequest *request);
    void systemReset(AsyncWebServerRequest *request);
    void modbus(AsyncWebServerRequest *request);
    void modbusValue(AsyncWebServerRequest *request);
    void modbusConfig(AsyncWebServerRequest *request);
    void serial1Config(AsyncWebServerRequest *request);

  private:
    Utils utils;
};

#endif
