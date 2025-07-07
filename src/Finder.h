
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

#ifndef FINDER_H
#define FINDER_H

#include <Arduino.h>
#include <ModbusMaster.h>

enum class FinderType { FOO, T1, T2, T3, T_float };

class Finder {
  public:
    Finder();
    void begin(Stream * serial, ModbusMaster * modbus);
    String typeToString(FinderType value);
    FinderType stringToType(String value);
    String getModbus(uint8_t deviceAddress, uint8_t functionCode, uint16_t registerAddress, FinderType valueType);

  private:
    Stream * serial;
    ModbusMaster * modbus;
    bool functionCode4_T1(uint16_t addr, uint16_t * value);
    bool functionCode4_T2(uint16_t addr, int16_t * value);
    bool functionCode4_T3(uint16_t addr, int32_t * value);
    bool functionCode4_T_float(uint16_t addr, float * value);
};

#endif
