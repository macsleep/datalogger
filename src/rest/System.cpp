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

void REST::System::body(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) request->_tempObject = malloc(total);
    if(len) memcpy((uint8_t *) (request->_tempObject) + index, data, len);
    request->send(200);
}
