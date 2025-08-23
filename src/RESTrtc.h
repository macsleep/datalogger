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

#ifndef RESTRTC_H
#define RESTRTC_H

#include <Arduino.h>
#include <RTClib.h>
#include <Regexp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Settings.h"

extern RTC_PCF8563 rtc;
extern Settings settings;

class RESTrtc {
  public:
    RESTrtc();
    void begin(AsyncWebServer *httpd);
    void rtcRequest(AsyncWebServerRequest * request);
    void rtcBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

  private:
};

#endif
