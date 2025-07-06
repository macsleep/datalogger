
/*
 * Finder Modbus RTU functions
 */

#ifndef FINDER_H
#define FINDER_H

#include <Arduino.h>
#include <ModbusMaster.h>

enum class FinderType { FOO, T1, T2, T3, T_float };

class Finder {
  public:
    Finder();
    void begin(ModbusMaster *modbus);
    String typeToString(FinderType value);
    FinderType stringToType(String value);
    bool functionCode4_T1(uint16_t addr, uint16_t *value);
    bool functionCode4_T2(uint16_t addr, int16_t *value);
    bool functionCode4_T3(uint16_t addr, int32_t *value);
    bool functionCode4_T_float(uint16_t addr, float *value);

  private:
     ModbusMaster *modbus;
};

#endif
