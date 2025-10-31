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
              std::bind(&Value::request, this, std::placeholders::_1));
}

void REST::Value::request(AsyncWebServerRequest *request) {
    String value = "";
    bool json = false;
    JsonDocument document;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;
    ModbusConfig config;

    // get NVS config
    int i = request->pathArg(0).toInt();
    settings.getModbusConfig(i, &config);

    // override values if available
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

    energyMeter.getModbus(&value, config.deviceAddress, config.functionCode, config.registerAddress, config.valueType);

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

