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

#ifndef TIMER_H
#define TIMER_H

#include <regex>
#include <Arduino.h>
#include <RTClib.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Settings.h"
#include "Timer_PFC8563.h"

extern Settings settings;
extern Timer_PFC8563 timer;

namespace REST {
    class Timer {
        public:
            Timer();
            void begin(AsyncWebServer * httpd);
            void request(AsyncWebServerRequest * request);
            void body(AsyncWebServerRequest * request, uint8_t * data, size_t len, size_t index, size_t total);

        private:
    };
}
#endif
