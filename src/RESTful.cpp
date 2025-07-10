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

#include "RESTful.h"

RESTful::RESTful() {
}

void RESTful::begin(AsyncWebServer *httpd) {
    httpd->on("^\\/api\\/rtc$", std::bind(&RESTful::rtcConfig, this, std::placeholders::_1));
    httpd->on("^\\/api\\/logs$", HTTP_GET, std::bind(&RESTful::logsList, this, std::placeholders::_1));
    httpd->on("^\\/api\\/logs\\/([0-9][0-9][0-9][0-9])([0-9][0-9][0-9][0-9])$", HTTP_GET,
              std::bind(&RESTful::logsFile, this, std::placeholders::_1));
    httpd->on("^\\/api\\/logs\\/([0-9][0-9][0-9][0-9])([0-9][0-9][0-9][0-9])$", HTTP_POST,
	      std::bind(&RESTful::logsFile, this, std::placeholders::_1),
	      std::bind(&RESTful::logsFileChunks, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    httpd->on("^\\/api\\/firmware\\/upload$", HTTP_POST,
	      std::bind(&RESTful::firmwareUpload, this, std::placeholders::_1),
	      std::bind(&RESTful::firmwareUploadChunks, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    httpd->on("^\\/api\\/system$", std::bind(&RESTful::systemConfig, this, std::placeholders::_1));
    httpd->on("^\\/api\\/system\\/reset$", std::bind(&RESTful::systemReset, this, std::placeholders::_1));
    httpd->on("^\\/api\\/modbus$", HTTP_GET, std::bind(&RESTful::modbus, this, std::placeholders::_1));
    httpd->on("^\\/api\\/modbus\\/([0-9]+)$", std::bind(&RESTful::modbusValue, this, std::placeholders::_1));
    httpd->on("^\\/api\\/modbus\\/([0-9]+)\\/config$", std::bind(&RESTful::modbusConfig, this, std::placeholders::_1));
}

void RESTful::rtcConfig(AsyncWebServerRequest *request) {
    int i, minutes;
    unsigned long epoch = 0;
    String value;

    switch (request->method()) {
     case HTTP_GET:
	 epoch = rtc.now().unixtime();
	 value = "epoch=" + String(epoch, DEC) + "&";
	 value += "minutes=" + String(settings.getTimer(), DEC);
	 request->send(200, "application/x-www-form-urlencoded", value);
	 break;

     case HTTP_PUT:
	 if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
	     return request->requestAuthentication();

	 for(i = 0; i < request->params(); i++) {
	     if(String(request->getParam(i)->name()).equals("epoch")) {
	         value = String(request->getParam(i)->value());
	         rtc.adjust(DateTime(value.toInt()));
	     }

	     if(String(request->getParam(i)->name()).equals("minutes")) {
	         value = String(request->getParam(i)->value());
	         minutes = value.toInt();
	         if(minutes >= 0 && minutes < 256) {
		     if(timer.isEnabled()) timer.disable();
		     timer.enable(minutes);
		     settings.setTimer((uint8_t) minutes);
	         }
	     }
	 }

	 request->send(200);
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
		    response->println(String(directory.name()) + String(file.name()) + " " + String(file.size()));
		}
		file.close();
	    }
	}
	entry.close();
    }
    root.close();

    request->send(response);
}

void RESTful::logsFile(AsyncWebServerRequest *request) {
    String file;

    switch (request->method()) {
     case HTTP_GET:
	 file = "/" + request->pathArg(0) + "/" + request->pathArg(1);
	 if(SD.exists(file)) {
	     request->send(SD, file, "text/plain");
	 }
	 break;

     case HTTP_POST:
	 request->send(200);
	 break;

     default:
	 request->send(400);
	 break;
    }
}

void RESTful::logsFileChunks(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    String file;

    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
	return request->requestAuthentication();

    if(!index) {
	file = "/" + request->pathArg(0) + "/" + request->pathArg(1);
	if(SD.exists(file)) {
	    request->_tempFile = SD.open(file, "w");
	}
    }

    if(len) {
	request->_tempFile.write(data, len);
    }

    if(final) {
	request->_tempFile.close();
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

void RESTful::systemConfig(AsyncWebServerRequest *request) {
    int i;
    String value = "";
    String version = "???";

    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
	return request->requestAuthentication();

#ifdef GIT_HASH
    version = GIT_HASH;
#endif

    switch (request->method()) {
     case HTTP_GET:
	 value = value + "wifiSSID=" + settings.getWifiSSID() + "&";
	 value = value + "wifiPassword=" + settings.getWifiPassword() + "&";
	 value = value + "httpUser=" + settings.getHttpUser() + "&";
	 value = value + "httpPassword=" + settings.getHttpPassword() + "&";
	 value = value + "version=" + version;

	 request->send(200, "application/x-www-form-urlencoded", value);
	 break;

     case HTTP_PUT:
	 for(i = 0; i < request->params(); i++) {
	     if(String(request->getParam(i)->name()).equals("wifiSSID")) {
		 settings.setWifiSSID(String(request->getParam(i)->value()));
	     }
	     if(String(request->getParam(i)->name()).equals("wifiPassword")) {
		 settings.setWifiPassword(String(request->getParam(i)->value()));
	     }
	     if(String(request->getParam(i)->name()).equals("httpUser")) {
		 settings.setHttpUser(String(request->getParam(i)->value()));
	     }
	     if(String(request->getParam(i)->name()).equals("httpPassword")) {
		 settings.setHttpPassword(String(request->getParam(i)->value()));
	     }
	 }

	 request->send(200);
	 break;

     default:
	 request->send(400);
	 break;
    }
}

void RESTful::systemReset(AsyncWebServerRequest *request) {
    int i;
    String value = "";

    if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
	return request->requestAuthentication();

    switch (request->method()) {
     case HTTP_POST:
	 settings.reset();
	 request->send(200);
	 break;

     default:
	 request->send(400);
	 break;
    }
}

void RESTful::modbus(AsyncWebServerRequest *request) {
    int i;
    ModbusConfig config;
    AsyncResponseStream *response;

    response = request->beginResponseStream("text/html");

    i = 0;
    while(settings.getModbusConfig(i, &config)) {
	response->println(String(i));
	i++;
    }

    request->send(response);
}

void RESTful::modbusValue(AsyncWebServerRequest *request) {
    int n;
    ModbusConfig config;
    String value = "";

    n = request->pathArg(0).toInt();

    switch (request->method()) {
     case HTTP_GET:
	 if(settings.getModbusConfig(n, &config)) {
	     value = energyMeter.getModbus(config.deviceAddress, config.functionCode, config.registerAddress, config.valueType);
	     request->send(200, "text/plain", value);
	 } else request->send(400);
	 break;

     default:
	 request->send(400);
	 break;
    }
}

void RESTful::modbusConfig(AsyncWebServerRequest *request) {
    int n, i;
    ModbusConfig config;
    String value = "";

    n = request->pathArg(0).toInt();

    switch (request->method()) {
     case HTTP_GET:
	 if(settings.getModbusConfig(n, &config)) {
	     value = value + "deviceAddress=" + String(config.deviceAddress) + "&";
	     value = value + "functionCode=" + String(config.functionCode) + "&";
	     value = value + "registerAddress=" + String(config.registerAddress) + "&";
	     value = value + "valueType=" + energyMeter.typeToString(config.valueType);
	     request->send(200, "application/x-www-form-urlencoded", value);
	 } else request->send(400);
	 break;

     case HTTP_PUT:
	 if(!request->authenticate(settings.getHttpUser().c_str(), settings.getHttpPassword().c_str()))
	     return request->requestAuthentication();

	 if(settings.getModbusConfig(n, &config)) {
	     for(i = 0; i < request->params(); i++) {
		 if(String(request->getParam(i)->name()).equals("deviceAddress")) {
		     config.deviceAddress = request->getParam(i)->value().toInt();
		 }
		 if(String(request->getParam(i)->name()).equals("functionCode")) {
		     config.functionCode = request->getParam(i)->value().toInt();
		 }
		 if(String(request->getParam(i)->name()).equals("registerAddress")) {
		     config.registerAddress = request->getParam(i)->value().toInt();
		 }
		 if(String(request->getParam(i)->name()).equals("valueType")) {
		     config.valueType = energyMeter.stringToType(request->getParam(i)->value());
		 }
	     }
	     settings.setModbusConfig(n, &config);
	     request->send(200);
	 } else request->send(400);
	 break;

     default:
	 request->send(400);
	 break;
    }
}
