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

#include "API.h"

REST::API::API() {
}

void REST::API::begin(AsyncWebServer *httpd) {
    restRTC = new REST::RTC();
    restRTC->begin(httpd);

    restTimer = new REST::Timer();
    restTimer->begin(httpd);

    restLogs = new REST::Logs();
    restLogs->begin(httpd);

    restYear = new REST::Year();
    restYear->begin(httpd);

    restMonth = new REST::Month();
    restMonth->begin(httpd);

    restDay = new REST::Day();
    restDay->begin(httpd);

    restFirmware = new REST::Firmware();
    restFirmware->begin(httpd);

    restSystem = new REST::System();
    restSystem->begin(httpd);

    restModbus = new REST::Modbus();
    restModbus->begin(httpd);

    restValue = new REST::Value();
    restValue->begin(httpd);

    restConfig = new REST::Config();
    restConfig->begin(httpd);

    restSerial1 = new REST::Serial1();
    restSerial1->begin(httpd);

    restLogin = new REST::Login();
    restLogin->begin(httpd);
}
