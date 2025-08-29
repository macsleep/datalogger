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
    httpd->on("^\\/api\\/modbus\\/([0-9]+)\\/config$", HTTP_GET|HTTP_PUT, std::bind(&Config::request, this, std::placeholders::_1), NULL, NULL);
}

void REST::Config::request(AsyncWebServerRequest *request) {
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

