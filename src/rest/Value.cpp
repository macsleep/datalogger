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

#include "Value.h"

REST::Value::Value() {
}

void REST::Value::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/modbus\\/([0-9]+)\\/?$", HTTP_GET,
              std::bind(&Value::request, this, std::placeholders::_1), NULL,
              std::bind(&Value::body, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void REST::Value::request(AsyncWebServerRequest *request) {
    String value = "";
    bool json = false;
    JsonDocument document;
    DeserializationError error;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;
    ModbusConfig config;

    // get NVS config
    int i = request->pathArg(0).toInt();
    settings.getModbusConfig(i, &config);

    // override values if available
    error = deserializeJson(document, (const char *)(request->_tempObject));
    if(error) {
        if(request->hasParam("deviceAddress")) {
            config.deviceAddress = request->getParam("deviceAddress")->value().toInt();
        }
        if(request->hasParam("functionCode")) {
            config.functionCode = request->getParam("functionCode")->value().toInt();
        }
        if(request->hasParam("registerAddress")) {
            config.registerAddress = request->getParam("registerAddress")->value().toInt();
        }
        if(request->hasParam("valueType")) {
            config.valueType = utils.stringToType(request->getParam("valueType")->value());
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

    bool ok = energyMeter.getModbus(&value, config.deviceAddress, config.functionCode, config.registerAddress, config.valueType);

    if(request->hasHeader("Accept")) {
        header = request->getHeader("Accept");
        if(std::regex_search(header->value().c_str(), std::regex("application/json"))) {
            json = true;
        }
    }

    if(json) {
        response = request->beginResponseStream("application/json");
        document["value"] = value;
        serializeJson(document, *response);
        request->send(response);
    } else {
        request->send(200, "text/plain", value);
    }
}

void REST::Value::body(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) {
        request->_tempObject = malloc(total + 1);
        bzero(request->_tempObject, total + 1);
    }
    if(len && request->_tempObject != NULL) {
        memcpy((uint8_t *)(request->_tempObject) + index, data, len);
    }
}

