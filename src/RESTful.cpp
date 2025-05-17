
#include "RESTful.h"

RESTful::RESTful() {
}

void RESTful::rtcDate(AsyncWebServerRequest *request) {
    DateTime now;
    String value;
    unsigned long epoch = 0;

    switch (request->method()) {
     case HTTP_GET:
         now = rtc.now();
         epoch = now.unixtime();
         value = String(epoch, DEC);
         request->send(200, "text/plain", value);
         break;

     case HTTP_PUT:
         if(!request->authenticate("http", "acme"))
           return request->requestAuthentication();

         if(request->hasParam("epoch", true)) {
           value = request->getParam("epoch", true)->value();
           rtc.adjust(DateTime(value.toInt()));
           request->send(200);
         } else {
           request->send(400);
         }
         break;

     default:
         request->send(400);
         break;
    }
}

