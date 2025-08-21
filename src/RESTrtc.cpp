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

#include "RESTrtc.h"

RESTrtc::RESTrtc(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/rtc$", HTTP_GET, std::bind(&RESTrtc::onRequestGet, this, std::placeholders::_1));
    httpd->on("^\\/api\\/rtc$", HTTP_PUT, std::bind(&RESTrtc::onRequestPut, this, std::placeholders::_1), NULL,
              std::bind(&RESTrtc::onBodyPut, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void RESTrtc::onRequestGet(AsyncWebServerRequest *request) {
    String value;
    uint32_t epoch;
    MatchState regex;
    JsonDocument document;

    epoch = rtc.now().unixtime();
    request->send(200, "text/plain", String(epoch).c_str());

    if(!request->hasHeader("Accept")) return;
    const AsyncWebHeader *header = request->getHeader("Accept");
    regex.Target((char *) header->value().c_str());

    if(regex.Match("application/json")) {
        document["epoch"] = epoch;
        serializeJson(document, value);
        request->send(200, "application/json", value.c_str());
    }
}

void RESTrtc::onRequestPut(AsyncWebServerRequest *request) {
    uint32_t epoch;
    JsonDocument document;
    DeserializationError error;

    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
        return request->requestAuthentication();

    if(request->hasParam("epoch", true)) {
        const AsyncWebParameter *param = request->getParam("epoch", true);
        epoch = param->value().toInt();
        rtc.adjust(DateTime(epoch));
    }

    error = deserializeJson(document, (const char *)(request->_tempObject));
    if(!error) {
        if(document["epoch"].is<uint32_t>()) {
            epoch = document["epoch"];
            rtc.adjust(DateTime(epoch));
        }
    }

    request->send(200);
}

void RESTrtc::onBodyPut(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) request->_tempObject = malloc(total);

    if(len) memcpy((uint8_t *)(request->_tempObject) + index, data, len);

    request->send(200);
}
