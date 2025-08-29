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

#include "API.h"

REST::API::API() {
}

void REST::API::begin(AsyncWebServer *httpd) {
    restRtc.begin(httpd);
    restTimer.begin(httpd);
    restLogs.begin(httpd);
    restLogFile.begin(httpd);
    restFirmware.begin(httpd);

    // system settings
    httpd->on("^\\/api\\/system$", std::bind(&REST::API::systemConfig, this, std::placeholders::_1));

    // modbus registers
    httpd->on("^\\/api\\/modbus$", HTTP_GET, std::bind(&REST::API::modbus, this, std::placeholders::_1));
    httpd->on("^\\/api\\/modbus\\/([0-9]+)$", std::bind(&REST::API::modbusValue, this, std::placeholders::_1));
    httpd->on("^\\/api\\/modbus\\/([0-9]+)\\/config$", std::bind(&REST::API::modbusConfig, this, std::placeholders::_1));

    // serial configuration
    httpd->on("^\\/api\\/serial1$", std::bind(&REST::API::serial1Config, this, std::placeholders::_1));
}

void REST::API::systemConfig(AsyncWebServerRequest *request) {
    int i;
    String value = "";

    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
        return request->requestAuthentication();

    switch (request->method()) {
     case HTTP_GET:
         value = value + "wifiSSID=" + settings.getWifiSSID() + "&";
         value = value + "wifiPassword=" + settings.getWifiPassword() + "&";
         value = value + "httpUser=" + settings.getHttpUser() + "&";
         value = value + "httpPassword=" + settings.getHttpPassword();

         request->send(200, "application/x-www-form-urlencoded", value);
         break;

     case HTTP_PUT:
         for(i = 0; i < request->params(); i++) {
             if(request->getParam(i)->name().equals("wifiSSID")) {
                 settings.setWifiSSID(request->getParam(i)->value());
             }
             if(request->getParam(i)->name().equals("wifiPassword")) {
                 settings.setWifiPassword(request->getParam(i)->value());
             }
             if(request->getParam(i)->name().equals("httpUser")) {
                 settings.setHttpUser(request->getParam(i)->value());
             }
             if(request->getParam(i)->name().equals("httpPassword")) {
                 settings.setHttpPassword(request->getParam(i)->value());
             }
         }

         request->send(200);
         break;

     case HTTP_DELETE:
         settings.reset();

         request->send(200);
         break;

     default:
         request->send(400);
         break;
    }
}

void REST::API::modbus(AsyncWebServerRequest *request) {
    int i;
    ModbusConfig config;
    AsyncResponseStream *response;

    response = request->beginResponseStream("text/html");

    i = 0;
    while(settings.getModbusConfig(i, &config)) {
        response->println(String(i));
        i++;
    }

    request->send(response);
}

void REST::API::modbusValue(AsyncWebServerRequest *request) {
    int n, status;
    ModbusConfig config;
    String value = "";

    n = request->pathArg(0).toInt();

    switch (request->method()) {
     case HTTP_GET:
         status = 400;
         if(settings.getModbusConfig(n, &config)) {
             value = energyMeter.getModbus(config.deviceAddress, config.functionCode, config.registerAddress, config.valueType);
             status = 200;
         }
         request->send(status, "text/plain", value);
         break;

     default:
         request->send(400);
         break;
    }
}

void REST::API::modbusConfig(AsyncWebServerRequest *request) {
    int n, i, status;
    ModbusConfig config;
    String value = "";

    n = request->pathArg(0).toInt();

    switch (request->method()) {
     case HTTP_GET:
         status = 400;
         if(settings.getModbusConfig(n, &config)) {
             value = value + "deviceAddress=" + String(config.deviceAddress) + "&";
             value = value + "functionCode=" + String(config.functionCode) + "&";
             value = value + "registerAddress=" + String(config.registerAddress) + "&";
             value = value + "valueType=" + utils.typeToString(config.valueType);
             status = 200;
         }
         request->send(status, "application/x-www-form-urlencoded", value);
         break;

     case HTTP_PUT:
         if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
             return request->requestAuthentication();

         status = 400;
         if(settings.getModbusConfig(n, &config)) {
             for(i = 0; i < request->params(); i++) {
                 if(request->getParam(i)->name().equals("deviceAddress")) {
                     config.deviceAddress = request->getParam(i)->value().toInt();
                 }
                 if(request->getParam(i)->name().equals("functionCode")) {
                     config.functionCode = request->getParam(i)->value().toInt();
                 }
                 if(request->getParam(i)->name().equals("registerAddress")) {
                     config.registerAddress = request->getParam(i)->value().toInt();
                 }
                 if(request->getParam(i)->name().equals("valueType")) {
                     config.valueType = utils.stringToType(request->getParam(i)->value());
                 }
             }
             if(request->params()) {
                 settings.setModbusConfig(n, &config);
                 status = 200;
             }
         }
         request->send(status);
         break;

     default:
         request->send(400);
         break;
    }
}


void REST::API::serial1Config(AsyncWebServerRequest *request) {
    int i;
    String value = "";

    switch (request->method()) {
     case HTTP_GET:
         value = "baud=" + String(settings.getSerial1Baud()) + "&";
         value = value + "config=" + utils.configToString((SerialConfig) settings.getSerial1Config());
         request->send(200, "application/x-www-form-urlencoded", value);
         break;

     case HTTP_PUT:
         if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
             return request->requestAuthentication();

         for(i = 0; i < request->params(); i++) {
             if(request->getParam(i)->name().equals("baud")) {
                 settings.setSerial1Baud(request->getParam(i)->value().toInt());
             }
             if(request->getParam(i)->name().equals("config")) {
                 settings.setSerial1Config(utils.stringToConfig(request->getParam(i)->value()));
             }
         }
         request->send(200);
         break;

     default:
         request->send(400);
         break;
    }
}
