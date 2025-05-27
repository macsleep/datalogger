
/*
 * RESTful API functions
 */

#ifndef RESTFUL_H
#define RESTFUL_H

#include <Arduino.h>
#include <SD.h>
#include <RTClib.h>
#include <Regexp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Timer_PFC8563.h"
#include "Settings.h"

extern RTC_PCF8563 rtc;
extern Timer_PFC8563 timer;
extern Settings settings;

class RESTful {
  public:
    RESTful();
    void begin(AsyncWebServer *httpd);
    void rtcDate(AsyncWebServerRequest *request);
    void rtcTimer(AsyncWebServerRequest *request);
    void logsList(AsyncWebServerRequest *request);
    void logsFile(AsyncWebServerRequest *request);
    void firmwareUpload(AsyncWebServerRequest *request);
    void firmwareUploadChunks(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void system(AsyncWebServerRequest *request);
  private:
};

#endif
