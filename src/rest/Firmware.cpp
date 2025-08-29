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

#include "Firmware.h"

REST::Firmware::Firmware() {
}

void REST::Firmware::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/firmware$", HTTP_GET | HTTP_POST,
              std::bind(&REST::Firmware::request, this, std::placeholders::_1),
              std::bind(&REST::Firmware::upload, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
}

void REST::Firmware::request(AsyncWebServerRequest *request) {
    String file;
    String version = "???";

    switch (request->method()) {
     case HTTP_GET:
#ifdef GIT_HASH
         version = GIT_HASH;
#endif
         request->send(200, "text/plain", version);
         break;

     case HTTP_POST:
         request->send(200);
         break;

     default:
         request->send(400);
         break;
    }
}

void REST::Firmware::upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
        return request->requestAuthentication();

    if(!index) {
        request->_tempFile = SD.open("/firmware.bin", "w");
    }

    if(len) {
        request->_tempFile.write(data, len);
    }

    if(final) {
        request->_tempFile.close();
    }

}
