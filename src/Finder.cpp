
#include "Finder.h"

Finder::Finder() {
    Finder::modbus = NULL;
}

void Finder::begin(ModbusMaster *modbus) {
    Finder::modbus = modbus;
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
