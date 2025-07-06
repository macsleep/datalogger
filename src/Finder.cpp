
#include "Finder.h"

Finder::Finder() {
    Finder::modbus = NULL;
}

void Finder::begin(ModbusMaster *modbus) {
    Finder::modbus = modbus;
}

String Finder::typeToString(FinderType value) {
    switch(value) {
        case FinderType::T1: return "T1";
        case FinderType::T2: return "T2";
        case FinderType::T3: return "T3";
        case FinderType::T_float: return "T_float";
        default: return "FOO";
    }
}

FinderType Finder::stringToType(String value) {
    if(value.equals("T1")) return FinderType::T1;
    if(value.equals("T2")) return FinderType::T2;
    if(value.equals("T3")) return FinderType::T3;
    if(value.equals("T_float")) return FinderType::T_float;
    return(FinderType::FOO);
}

bool Finder::functionCode4_T2(uint16_t addr, int16_t *value) {
    uint8_t result;

    result = modbus->readInputRegisters(addr, 1);
    if(result != modbus->ku8MBSuccess) return (false);
    *value = modbus->getResponseBuffer(0);

    return (true);
}

bool Finder::functionCode4_T1(uint16_t addr, uint16_t *value) {
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
