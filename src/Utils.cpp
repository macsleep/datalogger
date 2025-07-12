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

#include "Utils.h"

Utils::Utils() {
}

String Utils::configToString(SerialConfig value) {
    String config;

    switch (value) {
     case SerialConfig::SERIAL_8N1:
	 config = "8N1";
	 break;
     case SerialConfig::SERIAL_8N2:
	 config = "8N2";
	 break;
     case SerialConfig::SERIAL_8E1:
	 config = "8E1";
	 break;
     case SerialConfig::SERIAL_8E2:
	 config = "8E2";
	 break;
     case SerialConfig::SERIAL_8O1:
	 config = "8O1";
	 break;
     case SerialConfig::SERIAL_8O2:
	 config = "8O2";
	 break;

     case SerialConfig::SERIAL_7N1:
	 config = "7N1";
	 break;
     case SerialConfig::SERIAL_7N2:
	 config = "7N2";
	 break;
     case SerialConfig::SERIAL_7E1:
	 config = "7E1";
	 break;
     case SerialConfig::SERIAL_7E2:
	 config = "7E2";
	 break;
     case SerialConfig::SERIAL_7O1:
	 config = "7O1";
	 break;
     case SerialConfig::SERIAL_7O2:
	 config = "7O2";
	 break;

     default:
	 config = "8N1";
	 break;
    }

    return (config);
}

SerialConfig Utils::stringToConfig(String value) {
    SerialConfig config = SERIAL_8N1;

    if(value.equals("8N1")) config = SERIAL_8N1;
    if(value.equals("8N2")) config = SERIAL_8N2;
    if(value.equals("8E1")) config = SERIAL_8E1;
    if(value.equals("8E2")) config = SERIAL_8E2;
    if(value.equals("8O1")) config = SERIAL_8O1;
    if(value.equals("8O2")) config = SERIAL_8O2;

    if(value.equals("7N1")) config = SERIAL_7N1;
    if(value.equals("7N2")) config = SERIAL_7N2;
    if(value.equals("7E1")) config = SERIAL_7E1;
    if(value.equals("7E2")) config = SERIAL_7E2;
    if(value.equals("7O1")) config = SERIAL_7O1;
    if(value.equals("7O2")) config = SERIAL_7O2;

    return (config);
}

String Utils::typeToString(FinderType value) {
    String type;

    switch (value) {
     case FinderType::T1:
	 type = "T1";
	 break;
     case FinderType::T2:
	 type = "T2";
	 break;
     case FinderType::T3:
	 type = "T3";
	 break;
     case FinderType::T_float:
	 type = "T_float";
	 break;
     default:
	 type = "FOO";
	 break;
    }

    return (type);
}

FinderType Utils::stringToType(String value) {
    FinderType type = FinderType::FOO;

    if(value.equals("T1")) type = FinderType::T1;
    if(value.equals("T2")) type = FinderType::T2;
    if(value.equals("T3")) type = FinderType::T3;
    if(value.equals("T_float")) type = FinderType::T_float;

    return (type);
}
