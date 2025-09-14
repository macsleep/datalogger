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

#include "Year.h"

REST::Year::Year() {
}

void REST::Year::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/logs\\/([0-9][0-9][0-9][0-9])$", HTTP_GET | HTTP_DELETE,
              std::bind(&Year::request, this, std::placeholders::_1));
}

void REST::Year::request(AsyncWebServerRequest *request) {
    bool json = false;
    JsonDocument document;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;
    String path = "/" + request->pathArg(0);
    std::set<String> *months;

    switch(request->method()) {
        case HTTP_GET:
            months = utils.listDirs(path, std::regex("^[0-9][0-9]$"));

            if(request->hasHeader("Accept")) {
                header = request->getHeader("Accept");
                if(std::regex_search(header->value().c_str(), std::regex("application/json"))) {
                    json = true;
                }
            }

            if(json) {
                response = request->beginResponseStream("application/json");
                JsonArray array = document["months"].to<JsonArray>();
                for(auto month:(*months)) array.add(month);
                document.shrinkToFit();
                serializeJson(document, *response);
                request->send(response);
            } else {
                response = request->beginResponseStream("text/html");
                for(auto month:(*months)) response->println(month);
                request->send(response);
            }

            (*months).clear();
            break;

        case HTTP_DELETE:
            if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
                return request->requestAuthentication();

            if(SD.exists(path) && SD.rmdir(path)) {
                request->send(200);
            } else request->send(400);
            break;

        default:
            request->send(400);
            break;
    }
}

