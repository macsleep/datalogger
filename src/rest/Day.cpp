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

#include "Day.h"

REST::Day::Day() {
}

void REST::Day::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/logs\\/([0-9][0-9][0-9][0-9])\\/([0-9][0-9])\\/([0-9][0-9])\\/?$", HTTP_GET | HTTP_DELETE | HTTP_POST,
              std::bind(&Day::request, this, std::placeholders::_1),
              std::bind(&Day::upload, this, std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
}

void REST::Day::request(AsyncWebServerRequest *request) {
    String file = "/" + request->pathArg(0) + "/" + request->pathArg(1) + "/" + request->pathArg(2);

    switch(request->method()) {
        case HTTP_GET:
            if(SD.exists(file)) {
                request->send(SD, file, "text/plain");
            } else request->send(400);
            break;

        case HTTP_DELETE:
            if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
                return request->requestAuthentication();

            if(SD.exists(file)) {
                SD.remove(file);
                request->send(200);
            } else request->send(400);
            break;

        case HTTP_POST:
            request->send(200);
            break;

        default:
            request->send(400);
            break;
    }
}

void REST::Day::upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    String file = "/" + request->pathArg(0) + "/" + request->pathArg(1) + "/" + request->pathArg(2);

    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
        return request->requestAuthentication();

    if(!index) {
        if(SD.exists(file)) {
            request->_tempFile = SD.open(file, FILE_WRITE);
        }
    }

    if(len) {
        request->_tempFile.write(data, len);
    }

    if(final) {
        request->_tempFile.close();
    }
}
