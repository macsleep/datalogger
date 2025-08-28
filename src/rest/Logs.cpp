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

#include "Logs.h"

REST::Logs::Logs() {
}

void REST::Logs::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/logs$", HTTP_GET, std::bind(&Logs::request, this, std::placeholders::_1));
}

void REST::Logs::request(AsyncWebServerRequest *request) {
    MatchState regex;
    JsonDocument document;
    const AsyncWebHeader *header;
    AsyncResponseStream *response;

    std::map < String, int >*logs = listLong();

    if(!request->hasHeader("Accept")) return;
    header = request->getHeader("Accept");
    regex.Target((char *) header->value().c_str());

    if(regex.Match("application/json")) {
        response = request->beginResponseStream("application/json");
      for(auto log:(*logs)) document[log.first] = log.second;
        serializeJson(document, *response);
        request->send(response);
    } else {
        response = request->beginResponseStream("text/html");
      for(auto log:(*logs)) response->println(log.first + " " + String(log.second));
        request->send(response);
    }

    (*logs).clear();
}

std::map < String, int >*REST::Logs::listLong(void) {
    File root, entry, directory, file;
    MatchState regex;
    std::map < String, int >*logs = new std::map < String, int >;

    root = SD.open("/");
    while(entry = root.openNextFile()) {
        regex.Target((char *) entry.name());
        if(entry.isDirectory() && regex.Match("^[0-9][0-9][0-9][0-9]$")) {
            directory = SD.open("/" + String(entry.name()));
            while(file = directory.openNextFile()) {
                regex.Target((char *) file.name());
                if(!file.isDirectory() && regex.Match("^[0-9][0-9][0-9][0-9]$")) {
                    (*logs)[String(directory.name()) + String(file.name())] = file.size();
                }
                file.close();
            }
            directory.close();
        }
        entry.close();
    }
    root.close();

    return (logs);
}
