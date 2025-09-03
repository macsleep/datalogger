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

#include "Serial1.h"

REST::Serial1::Serial1() {
}

void REST::Serial1::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/serial1$", HTTP_GET | HTTP_PUT, std::bind(&Serial1::request, this, std::placeholders::_1), NULL,
              std::bind(&Serial1::body, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void REST::Serial1::request(AsyncWebServerRequest *request) {
    String value = "";
    bool json = false;
    JsonDocument document;
    DeserializationError error;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;

    switch(request->method()) {
        case HTTP_GET:

            if(request->hasHeader("Accept")) {
                header = request->getHeader("Accept");
                if(std::regex_match(header->value().c_str(), std::regex("application/json"))) {
                    json = true;
                }
            }

            if(json) {
                response = request->beginResponseStream("application/json");
                document["baud"] = settings.getSerial1Baud();
                document["config"] = utils.configToString((SerialConfig) settings.getSerial1Config());
                serializeJson(document, *response);
                request->send(response);
            } else {
                value = "baud=" + String(settings.getSerial1Baud()) + "&";
                value = value + "config=" + utils.configToString((SerialConfig) settings.getSerial1Config());
                request->send(200, "application/x-www-form-urlencoded", value);
            }
            break;

        case HTTP_PUT:
            if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
                return request->requestAuthentication();

            error = deserializeJson(document, (const char *)(request->_tempObject));
            if(error) {
                if(request->hasParam("baud", true)) {
                    settings.setSerial1Baud(request->getParam("baud", true)->value().toInt());
                }
                if(request->hasParam("config", true)) {
                    settings.setSerial1Config(utils.stringToConfig(request->getParam("config", true)->value()));
                }
            } else {
                if(document["baud"].is<uint32_t>()) {
                    settings.setSerial1Baud(document["baud"]);
                }
                if(document["config"].is<String>()) {
                    settings.setSerial1Config(utils.stringToConfig(document["config"]));
                }
            }

            request->send(200);
            break;

        default:
            request->send(400);
            break;
    }
}

void REST::Serial1::body(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) {
        request->_tempObject = malloc(total + 1);
        bzero(request->_tempObject, total + 1);
    }
    if(len && request->_tempObject != NULL) {
        memcpy((uint8_t *)(request->_tempObject) + index, data, len);
    }
}

