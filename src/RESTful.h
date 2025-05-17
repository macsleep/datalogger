
/*
 * RESTful API functions
 */

#ifndef RESTFUL_H
#define RESTFUL_H

#include <Arduino.h>
#include <RTClib.h>
#include <Regexp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern RTC_PCF8563 rtc;

class RESTful {
    public:
        RESTful();
        void rtcDate(AsyncWebServerRequest *request);
    private:
};

#endif
