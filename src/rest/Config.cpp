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

#include "Config.h"

REST::Config::Config() {
}

void REST::Config::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/modbus\\/([0-9]+)\\/config\\/?$", HTTP_GET | HTTP_PUT,
              std::bind(&Config::request, this, std::placeholders::_1), NULL,
              std::bind(&Config::body, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void REST::Config::request(AsyncWebServerRequest *request) {
    String value = "";
    bool json = false, ok;
    JsonDocument document;
    DeserializationError error;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;
    ModbusConfig config;

    int i = request->pathArg(0).toInt();
    ok = settings.getModbusConfig(i, &config);

    switch(request->method()) {
        case HTTP_GET:
            if(request->hasHeader("Accept")) {
                header = request->getHeader("Accept");
                if(std::regex_search(header->value().c_str(), std::regex("application/json"))) {
                    json = true;
                }
            }

            if(json) {
                if(ok) {
                    response = request->beginResponseStream("application/json");
                    document["deviceAddress"] = config.deviceAddress;
                    document["functionCode"] = config.functionCode;
                    document["registerAddress"] = config.registerAddress;
                    document["valueType"] = utils.typeToString(config.valueType);
                    serializeJson(document, *response);
                    request->send(response);
                } else request->send(400);
            } else {
                if(ok) {
                    value = value + "deviceAddress=" + String(config.deviceAddress) + "&";
                    value = value + "functionCode=" + String(config.functionCode) + "&";
                    value = value + "registerAddress=" + String(config.registerAddress) + "&";
                    value = value + "valueType=" + utils.typeToString(config.valueType);
                    request->send(200, "application/x-www-form-urlencoded", value);
                } else request->send(400);
            }
            break;

        case HTTP_PUT:
            if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
                return request->requestAuthentication();

            error = deserializeJson(document, (const char *)(request->_tempObject));
            if(error) {
                if(request->hasParam("deviceAddress", true)) {
                    config.deviceAddress = request->getParam("deviceAddress", true)->value().toInt();
                }
                if(request->hasParam("functionCode", true)) {
                    config.functionCode = request->getParam("functionCode", true)->value().toInt();
                }
                if(request->hasParam("registerAddress", true)) {
                    config.registerAddress = request->getParam("registerAddress", true)->value().toInt();
                }
                if(request->hasParam("valueType", true)) {
                    config.valueType = utils.stringToType(request->getParam("valueType", true)->value());
                }
            } else {
                if(document["deviceAddress"].is<uint8_t>()) {
                    config.deviceAddress = document["deviceAddress"];
                }
                if(document["functionCode"].is<uint8_t>()) {
                    config.functionCode = document["functionCode"];
                }
                if(document["registerAddress"].is<uint16_t>()) {
                    config.registerAddress = document["registerAddress"];
                }
                if(document["valueType"].is<String>()) {
                    config.valueType = utils.stringToType(document["valueType"]);
                }
            }

            if(ok) {
                settings.setModbusConfig(i, &config);
                request->send(200);
            } else request->send(400);
            break;

        default:
            request->send(400);
            break;
    }
}

void REST::Config::body(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) {
        request->_tempObject = malloc(total + 1);
        bzero(request->_tempObject, total + 1);
    }
    if(len && request->_tempObject != NULL) {
        memcpy((uint8_t *)(request->_tempObject) + index, data, len);
    }
}

