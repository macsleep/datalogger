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

#include "RESTtimer.h"

RESTtimer::RESTtimer() {
}

void RESTtimer::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/timer$", HTTP_GET | HTTP_PUT,
              std::bind(&RESTtimer::timerRequest, this, std::placeholders::_1), NULL,
              std::bind(&RESTtimer::timerBody, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void RESTtimer::timerRequest(AsyncWebServerRequest *request) {
    String value;
    uint8_t minutes;
    MatchState regex;
    JsonDocument document;
    const AsyncWebHeader *header;
    const AsyncWebParameter *param;
    DeserializationError error;
    bool ok = false;

    switch (request->method()) {
     case HTTP_GET:
         minutes = settings.getTimer();

         if(!request->hasHeader("Accept")) return;
         header = request->getHeader("Accept");
         regex.Target((char *) header->value().c_str());

         if(regex.Match("application/json")) {
             document["minutes"] = minutes;
             serializeJson(document, value);
             request->send(200, "application/json", value.c_str());
         } else {
             request->send(200, "text/plain", String(minutes).c_str());
         }
         break;


     case HTTP_PUT:
         if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
             return request->requestAuthentication();

         error = deserializeJson(document, (const char *) (request->_tempObject));
         if(error) {
             if(request->hasParam("minutes", true)) {
                 param = request->getParam("minutes", true);
                 if(param->value().toInt() < 0x100) {
                     minutes = param->value().toInt();
                     ok = true;
                 }
             }
         } else {
             if(document["minutes"].is < uint8_t > ()) {
                 minutes = document["minutes"];
                 ok = true;
             }
         }

         if(ok) {
             if(timer.isEnabled()) timer.disable();
             timer.enable(minutes);
             settings.setTimer(minutes);
         }

         request->send(200);
         break;

     default:
         request->send(400);
         break;
    }
}

void RESTtimer::timerBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) request->_tempObject = malloc(total);
    if(len) memcpy((uint8_t *) (request->_tempObject) + index, data, len);
    request->send(200);
}
