
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

#define REST_USER "http"
#define REST_PASSWD "acme"

extern RTC_PCF8563 rtc;

class RESTful {
  public:
    RESTful();
    void rtcDate(AsyncWebServerRequest * request);
    void logsList(AsyncWebServerRequest *request);
    void logsFile(AsyncWebServerRequest *request);
    void firmwareUpload(AsyncWebServerRequest *request);
    void firmwareUploadChunks(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
  private:
};

#endif
