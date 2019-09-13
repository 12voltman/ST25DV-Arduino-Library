//============================================================================
// Name        : ST25DV.cpp
// Author      : Max Robinson
// Version     :
// Copyright   : Public Domain
// Description : Arduino library to interface with the ST25DV**K series of
//               dynamic NFC tags by ST. This library was created using the
//				 datasheet available at the following address:
//				 https://www.st.com/content/ccc/resource/technical/document/datasheet/group3/74/20/c5/ca/b8/a1/41/e3/DM00167716/files/DM00167716.pdf/jcr:content/translations/en.DM00167716.pdf
//============================================================================

#include "ST25DV.h"

//Constructors
    void ST25DV::ST25DV(){/*null constructor*/}

    uint8_t ST25DV::begin(TwoWire &portin){
        this->WIREPORT = &portin;
        uint8_t result = this->WIREPORT->begin();
        switch (getSizeK())
        {
        case 04:
            this->MEMENDPOINT = this->REG_USER_MEM_END_04K;
            break;
        case 16:
            this->MEMENDPOINT = this->REG_USER_MEM_END_16K;
            break;
        case 64:
            this->MEMENDPOINT = this->REG_USER_MEM_END_64K;
            break;
        }
        return result;
    }   



//Worker functions
    
    uint64_t ST25DV::get64bits(uint8_t add, uint16_t reg){
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        this->WIREPORT->endTransmission();
        this->WIREPORT->requestFrom(add, 8);
        uint64_t buffer = 0;
        buffer |= this->WIREPORT->read();
        for(uint8_t i = 0; i < 7; i++){
            buffer <<= 8;
            buffer |= this->WIREPORT->read();
        }
        this->WIREPORT->endTransmission();
        return buffer;
    }

    uint16_t ST25DV::get16bits(uint8_t add, uint16_t reg){
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        this->WIREPORT->endTransmission();
        this->WIREPORT->requestFrom(add, 8);
        uint16_t buffer = 0;
        buffer |= this->WIREPORT->read();
        buffer <<= 8;
        buffer |= this->WIREPORT->read();
        this->WIREPORT->endTransmission();
        return buffer;
    }
    
    uint8_t ST25DV::getByte(uint8_t add, uint16_t reg){
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        this->WIREPORT->endTransmission();
        this->WIREPORT->requestFrom(add, 1);
        uint8_t buffer = this->WIREPORT->read();
        this->WIREPORT->endTransmission();
        return buffer;
    }
    
    bool ST25DV::getBit(uint8_t add, uint8_t reg, uint8_t bit){
        uint8_t buffer = getByte(add, reg);
        buffer >>= bit;
        buffer &= 0x01;
        return buffer;
    }

    void ST25DV::set64bits(uint8_t add, uint16_t reg, uint64_t dat){
        array64bits adat;
        adat.d64 = dat;
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        for(uint8_t i = 0; i<8; i++){
            this->WIREPORT->write(adat.d8[7-i]);
        }
        this->WIREPORT->endTransmission();
    }

    void ST25DV::set16bits(uint8_t add, uint16_t reg, uint16_t dat){
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        this->WIREPORT->write(dat >> 8);
        this->WIREPORT->write(dat & 0xFF);
        this->WIREPORT->endTransmission();
    }

    void ST25DV::setByte(uint8_t add, uint16_t reg, uint8_t dat){
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        this->WIREPORT->write(dat);
        this->WIREPORT->endTransmission();
    }

    void ST25DV::setBit(uint8_t add, uint8_t reg, uint8_t bit, bool dat){
        uint8_t mask = 0x01 << bit;
        uint8_t buffer = getByte(add, reg);
        buffer = dat ? buffer | mask : buffer & ~mask;
        setByte(add, reg, buffer);
    }

//User memory functions





//Dynamic register functions
    
    
    
    

//Fast transfer mode buffer functions





//System configuration area functions
