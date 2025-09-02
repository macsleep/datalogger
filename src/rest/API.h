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

#ifndef API_H
#define API_H

#include <regex>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "rest/RTC.h"
#include "rest/Timer.h"
#include "rest/Logs.h"
#include "rest/Logfile.h"
#include "rest/Firmware.h"
#include "rest/System.h"
#include "rest/Modbus.h"
#include "rest/Value.h"
#include "rest/Config.h"
#include "rest/Serial1.h"

namespace REST {
    class API {
      public:
        API();
        void begin(AsyncWebServer * httpd);

      private:
        RTC *restRTC;
        Timer *restTimer;
        Logs *restLogs;
        Logfile *restLogfile;
        Firmware *restFirmware;
        System *restSystem;
        Modbus *restModbus;
        Value *restValue;
        Config *restConfig;
        Serial1 *restSerial1;
    };
}
#endif
