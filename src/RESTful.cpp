
#include "RESTful.h"

RESTful::RESTful() {
}

void RESTful::createURLs(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/rtc\\/date$",
              std::bind(&RESTful::rtcDate, this, std::placeholders::_1));
    httpd->on("^\\/api\\/logs$", HTTP_GET,
              std::bind(&RESTful::logsList, this, std::placeholders::_1));
    httpd->on("^\\/api\\/logs\\/([0-9][0-9][0-9][0-9])([0-9][0-9][0-9][0-9])$",
         HTTP_GET, std::bind(&RESTful::logsFile, this, std::placeholders::_1));
    httpd->on("^\\/api\\/firmware\\/upload$", HTTP_POST,
              std::bind(&RESTful::firmwareUpload, this, std::placeholders::_1),
              std::bind(&RESTful::firmwareUploadChunks, this, std::placeholders::_1,
              std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
              std::placeholders::_5, std::placeholders::_6));
}

void RESTful::rtcDate(AsyncWebServerRequest *request) {
    int status;
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
	 if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
	     return request->requestAuthentication();

	 status = 400;
	 if(request->hasParam("epoch", true)) {
	     value = request->getParam("epoch", true)->value();
	     rtc.adjust(DateTime(value.toInt()));
	     status = 200;
	 }
	 request->send(status);
	 break;

     default:
	 request->send(400);
	 break;
    }
}

void RESTful::logsList(AsyncWebServerRequest *request) {
    File root, entry, directory, file;
    AsyncResponseStream *response;
    MatchState regex;

    response = request->beginResponseStream("text/html");

    root = SD.open("/");
    while(entry = root.openNextFile()) {
	regex.Target((char *) entry.name());
	if(entry.isDirectory() && regex.Match("^[0-9][0-9][0-9][0-9]$")) {
	    directory = SD.open("/" + String(entry.name()));
	    while(file = directory.openNextFile()) {
		regex.Target((char *) file.name());
		if(!file.isDirectory() && regex.Match("^[0-9][0-9][0-9][0-9]$")) {
		    response->println(String(directory.name()) +
				      String(file.name()) + " " +
				      String(file.size()));
		}
		file.close();
	    }
	    entry.close();
	}
	root.close();
    }

    request->send(response);
}

void RESTful::logsFile(AsyncWebServerRequest *request) {
    String file;

    file = "/" + request->pathArg(0) + "/" + request->pathArg(1);
    if(SD.exists(file)) {
	request->send(SD, file, "text/plain");
    }
}

void RESTful::firmwareUpload(AsyncWebServerRequest *request) {
    request->send(200);
}

void RESTful::firmwareUploadChunks(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
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

