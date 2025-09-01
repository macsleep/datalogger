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

#include "System.h"

REST::System::System() {
}

void REST::System::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/system$", HTTP_GET | HTTP_PUT | HTTP_DELETE,
              std::bind(&System::request, this, std::placeholders::_1), NULL,
              std::bind(&System::body, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void REST::System::request(AsyncWebServerRequest *request) {
    String value = "";
    bool ok = false;
    JsonDocument document;
    DeserializationError error;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;

    switch (request->method()) {
     case HTTP_GET:

         if(request->hasHeader("Accept")) {
             header = request->getHeader("Accept");
             if(std::regex_match(header->value().c_str(), std::regex("application/json"))) {
                 ok = true;
             }
         }

         if(ok) {
             response = request->beginResponseStream("application/json");
             document["wifiSSID"] = settings.getWifiSSID();
             document["wifiPassword"] = settings.getWifiPassword();
             document["httpUser"] = settings.getHttpUser();
             document["httpPassword"] = settings.getHttpPassword();
             serializeJson(document, *response);
             request->send(response);
         } else {
             value = value + "wifiSSID=" + settings.getWifiSSID() + "&";
             value = value + "wifiPassword=" + settings.getWifiPassword() + "&";
             value = value + "httpUser=" + settings.getHttpUser() + "&";
             value = value + "httpPassword=" + settings.getHttpPassword();
             request->send(200, "application/x-www-form-urlencoded", value);
         }
         break;

     case HTTP_PUT:
         if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
             return request->requestAuthentication();

         error = deserializeJson(document, (const char *) (request->_tempObject));
         if(error) {
             if(request->hasParam("wifiSSID", true)) {
                 settings.setWifiSSID(request->getParam("wifiSSID", true)->value());
             }
             if(request->hasParam("wifiPassword", true)) {
                 settings.setWifiSSID(request->getParam("wifiPassword", true)->value());
             }
             if(request->hasParam("httpUser", true)) {
                 settings.setWifiSSID(request->getParam("httpUser", true)->value());
             }
             if(request->hasParam("httpPassword", true)) {
                 settings.setWifiSSID(request->getParam("httpPassword", true)->value());
             }
         } else {
             if(document["wifiSSID"].is<String>()) {
                 settings.setWifiSSID(document["wifiSSID"]);
             }
             if(document["wifiPassword"].is<String>()) {
                 settings.setWifiPassword(document["wifiPassword"]);
             }
             if(document["httpUser"].is<String>()) {
                 settings.setHttpUser(document["httpUser"]);
             }
             if(document["httpPassword"].is<String>()) {
                 settings.setHttpPassword(document["httpPassword"]);
             }
         }

         request->send(200);
         break;

     default:
         request->send(400);
         break;
    }
}

void REST::System::body(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) {
        request->_tempObject = malloc(total + 1);
        bzero(request->_tempObject, total + 1);
    }
    if(len && request->_tempObject != NULL) {
        memcpy((uint8_t *) (request->_tempObject) + index, data, len);
    }
}

