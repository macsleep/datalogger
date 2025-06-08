
/*
 * Finder Modbus RTU functions
 */

#ifndef FINDER_H
#define FINDER_H

#include <Arduino.h>
#include <ModbusMaster.h>
#include "Settings.h"

extern Settings settings;

class Finder {
  public:
    Finder();
    void begin(ModbusMaster *modbus);
    bool functionCode4_T3(uint16_t addr, int32_t * value);
    bool functionCode4_T_float(uint16_t addr, float *value);

  private:
     ModbusMaster *modbus;
};

#endif
