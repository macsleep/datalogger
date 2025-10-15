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

#include "Login.h"

REST::Login::Login() {
}

void REST::Login::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/login\\/?$", HTTP_POST, std::bind(&Login::request, this, std::placeholders::_1), NULL,
              std::bind(&REST::Login::body, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void REST::Login::request(AsyncWebServerRequest *request) {
    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
        return request->requestAuthentication();
    request->send(200);
}

void REST::Login::body(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(!index) {
        request->_tempObject = malloc(total + 1);
        bzero(request->_tempObject, total + 1);
    }
    if(len && request->_tempObject != NULL) {
        memcpy((uint8_t *)(request->_tempObject) + index, data, len);
    }
}

