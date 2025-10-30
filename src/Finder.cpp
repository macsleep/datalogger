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

#include "Finder.h"

Finder::Finder() {
    Finder::serial = NULL;
    Finder::modbus = NULL;
}

void Finder::begin(Stream *serial, ModbusMaster *modbus) {
    Finder::serial = serial;
    Finder::modbus = modbus;
}

bool Finder::getModbus(String *valueString, uint8_t deviceAddress, uint8_t functionCode, uint16_t registerAddress, FinderType valueType) {
    union Value value;
    char buffer[64];
    bool ok;

    if(deviceAddress > 0) {
        // device
        modbus->begin(deviceAddress, *Finder::serial);

        switch(functionCode) {
            case 4:
                switch(valueType) {
                    case FinderType::T1:
                        if(ok = functionCode4_T1(registerAddress, &value.uint16)) {
                            *valueString = String(value.uint16);
                        } else *valueString = "err";
                        break;

                    case FinderType::T2:
                        if(ok = functionCode4_T2(registerAddress, &value.int16)) {
                            *valueString = String(value.int16);
                        } else *valueString = "err";
                        break;

                    case FinderType::T3:
                        if(ok = functionCode4_T3(registerAddress, &value.int32)) {
                            *valueString = String(value.int32);
                        } else *valueString = "err";
                        break;

                    case FinderType::T_float:
                        if(ok = functionCode4_T_float(registerAddress, &value.float32)) {
                            snprintf(buffer, sizeof(buffer), "%g", value.float32);
                            *valueString = String(buffer);
                        } else *valueString = "err";
                        break;

                    default:
                        *valueString = "nyi";
                        ok = false;
                        break;
                }
                break;

            default:
                *valueString = "nyi";
                ok = false;
                break;
        }
    } else {
        *valueString = "una";
        ok = false;
    }

    return ok;
}

bool Finder::functionCode4_T1(uint16_t addr, uint16_t *value) {
    uint8_t result;

    result = modbus->readInputRegisters(addr, 1);
    if(result != modbus->ku8MBSuccess) return (false);
    *value = modbus->getResponseBuffer(0);

    return (true);
}

bool Finder::functionCode4_T2(uint16_t addr, int16_t *value) {
    uint8_t result;

    result = modbus->readInputRegisters(addr, 1);
    if(result != modbus->ku8MBSuccess) return (false);
    *value = modbus->getResponseBuffer(0);

    return (true);
}

bool Finder::functionCode4_T3(uint16_t addr, int32_t *value) {
    uint8_t result;
    uint16_t data[2];
    uint32_t i;

    result = modbus->readInputRegisters(addr, 2);
    if(result != modbus->ku8MBSuccess) return (false);
    for(i = 0; i < 2; i++) {
        data[i] = modbus->getResponseBuffer(i);
    }
    *value = (data[0] << 16) | data[1];

    return (true);
}

bool Finder::functionCode4_T_float(uint16_t addr, float *value) {
    uint8_t result, a, b, c, d;
    uint16_t data[2];
    uint32_t i;

    result = modbus->readInputRegisters(addr, 2);
    if(result != modbus->ku8MBSuccess) return (false);
    for(i = 0; i < 2; i++) {
        data[i] = modbus->getResponseBuffer(i);
    }

    a = (data[0] >> 8) & 0xff;
    b = (data[0] >> 0) & 0xff;
    c = (data[1] >> 8) & 0xff;
    d = (data[1] >> 0) & 0xff;

    i = (a << 24) | (b << 16) | (c << 8) | (d << 0);
    memcpy(value, &i, 4);

    return (true);
}
