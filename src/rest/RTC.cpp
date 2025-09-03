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

#include "RTC.h"

REST::RTC::RTC() {
}

void REST::RTC::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/rtc$", HTTP_GET | HTTP_PUT, std::bind(&RTC::request, this, std::placeholders::_1), NULL,
              std::bind(&RTC::body, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void REST::RTC::request(AsyncWebServerRequest *request) {
    bool json = false, ok = false;
    uint32_t epoch;
    JsonDocument document;
    DeserializationError error;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;

    switch(request->method()) {
        case HTTP_GET:
            epoch = rtc.now().unixtime();

            if(request->hasHeader("Accept")) {
                header = request->getHeader("Accept");
                if(std::regex_match(header->value().c_str(), std::regex("application/json"))) {
                    json = true;
                }
            }

            if(json) {
                response = request->beginResponseStream("application/json");
                document["epoch"] = epoch;
                serializeJson(document, *response);
                request->send(response);
            } else {
                request->send(200, "text/plain", String(epoch).c_str());
            }
            break;

        case HTTP_PUT:
            if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
                return request->requestAuthentication();

            error = deserializeJson(document, (const char *)(request->_tempObject));
            if(error) {
                if(request->hasParam("epoch", true)) {
                    const AsyncWebParameter *param = request->getParam("epoch", true);
                    epoch = param->value().toInt();
                    ok = true;
                }
            } else {
                if(document["epoch"].is < uint32_t > ()) {
                    epoch = document["epoch"];
                    ok = true;
                }
            }

            if(ok) rtc.adjust(DateTime(epoch));
            request->send(200);
            break;

        default:
            request->send(400);
            break;
    }
}

void REST::RTC::body(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) {
        request->_tempObject = malloc(total + 1);
        bzero(request->_tempObject, total + 1);
    }
    if(len && request->_tempObject != NULL) {
        memcpy((uint8_t *)(request->_tempObject) + index, data, len);
    }
}
