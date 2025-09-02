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
    httpd->on("^\\/api\\/serial1$", HTTP_GET | HTTP_PUT, std::bind(&Serial1::request, this, std::placeholders::_1));

}

void REST::Serial1::request(AsyncWebServerRequest *request) {
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
