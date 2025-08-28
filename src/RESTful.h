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

#ifndef FUL_H
#define FUL_H

#include <Arduino.h>
#include <SD.h>
#include <Regexp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HardwareSerial.h>
#include "Settings.h"
#include "Utils.h"
#include "rest/Rtc.h"
#include "rest/Timer.h"
#include "rest/Logs.h"

extern Finder energyMeter;;
extern Settings settings;

class RESTful {
  public:
    RESTful();
    void begin(AsyncWebServer * httpd);
    void logsFile(AsyncWebServerRequest * request);
    void logsFileChunks(AsyncWebServerRequest * request, String filename, size_t index, uint8_t * data, size_t len, bool final);
    void firmwareVersion(AsyncWebServerRequest * request);
    void firmwareUpload(AsyncWebServerRequest * request);
    void firmwareUploadChunks(AsyncWebServerRequest * request, String filename, size_t index, uint8_t * data, size_t len, bool final);
    void systemConfig(AsyncWebServerRequest * request);
    void modbus(AsyncWebServerRequest * request);
    void modbusValue(AsyncWebServerRequest * request);
    void modbusConfig(AsyncWebServerRequest * request);
    void serial1Config(AsyncWebServerRequest * request);

  private:
    REST::Rtc restRtc;
    REST::Timer restTimer;
    REST::Logs restLogs;
    Utils utils;
};

#endif
