//============================================================================
// Name        : ST25DV.cpp
// Author      : Max Robinson
// Date        : 27-09-2019
// Version     : 0.1
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
        this->BUILT_IN_DELAY = 1;
        uint8_t result = this->WIREPORT->begin();
        this->MEMENDPOINT = getLastAdd();
        return result;
    }

    void enableDelay(bool en){
        this->BUILT_IN_DELAY = en;
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
        if(this->BUILT_IN_DELAY){delay(18);}//Maximum EEPROM write time in ms (6ms * up to 3 pages to write)
    }

    void ST25DV::set16bits(uint8_t add, uint16_t reg, uint16_t dat){
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        this->WIREPORT->write(dat >> 8);
        this->WIREPORT->write(dat & 0xFF);
        this->WIREPORT->endTransmission();
        if(this->BUILT_IN_DELAY){delay(12);}//Maximum EEPROM write time in ms (6ms * up to 2 pages to write)
    }

    void ST25DV::setByte(uint8_t add, uint16_t reg, uint8_t dat){
        this->WIREPORT->beginTransmission(add);
        this->WIREPORT->write(reg >> 8);
        this->WIREPORT->write(reg & 0xFF);
        this->WIREPORT->write(dat);
        this->WIREPORT->endTransmission();
        if(this->BUILT_IN_DELAY){delay(6);}//Maximum EEPROM write time in ms (6ms * up to 1 page to write)
    }

    void ST25DV::setBit(uint8_t add, uint8_t reg, uint8_t bit, bool dat){
        uint8_t mask = 0x01 << bit;
        uint8_t buffer = getByte(add, reg);
        buffer = dat ? buffer | mask : buffer & ~mask;
        setByte(add, reg, buffer);
    }

    bool ST25DV::presentPassword(uint64_t pass){
        array64bits adat;
        adat.d64 = pass;
        this->WIREPORT->beginTransmission(this->ADDRESS_CONFIG);
        this->WIREPORT->write(this->REG_I2C_PWD_START >> 8);
        this->WIREPORT->write(this->REG_I2C_PWD_START & 0xFF);
        for(uint8_t i = 0; i<8; i++){
            this->WIREPORT->write(adat.d8[7-i]);
        }
        this->WIREPORT->write(this->REG_I2C_PWD_START >> 8);
        for(uint8_t i = 0; i<8; i++){
            this->WIREPORT->write(adat.d8[7-i]);
        }
        this->WIREPORT->endTransmission();
        if(this->BUILT_IN_DELAY){//Password comparison check delay and unlock verification
            delay(10);
            return getI2CUnlocked();
        }
        return 1;
    }

//User memory functions
    /*uint8_t* ST25DV::read(uint16_t reg, uint8_t len){
        return getBulk(this->ADDRESS, reg, len);
    }

    void ST25DV::write(uint16_t reg, uint8_t len, uint8_t* dat){
        setBulk(this->ADDRESS, reg, len, dat);
    }*/
    
    uint8_t ST25DV::readByte(uint16_t reg){
        if(reg < this->MEMENDPOINT){
            return getByte(this->ADDRESS, reg);
        }
        return 0;
    }

    void ST25DV::writeByte(uint16_t reg, uint8_t dat){
        if(reg < this->MEMENDPOINT){
            setByte(this->ADDRESS, reg, dat);
        }
    }

//Dynamic register functions
    bool ST25DV::getGPOEnabledDyn(){
        return getBit(this->ADDRESS, this->REG_GPO_CTRL_Dyn, 7);
    }
    
    void ST25DV::setGPOEnabledDyn(bool val){
        setBit(this->ADDRESS, this->REG_GPO_CTRL_Dyn, 7, val);
    }

    bool ST25DV::getEHEnabledDyn(){
        return getBit(this->ADDRESS, this->REG_EH_CONTROL_Dyn, 0);
    }

    void ST25DV::setEHEnabledDyn(bool val){
        setBit(this->ADDRESS, this->REG_EH_CONTROL_Dyn, 0, val);
    }

    bool ST25DV::getEHActive(){
        return getBit(this->ADDRESS, this->REG_EH_CONTROL_Dyn, 1);
    }

    bool ST25DV::getRFFieldPresent(){
        return getBit(this->ADDRESS, this->REG_EH_CONTROL_Dyn, 2);
    }

    bool ST25DV::getVCCOn(){
        return getBit(this->ADDRESS, this->REG_EH_CONTROL_Dyn, 3);
    }

    bool ST25DV::getRFDisableDyn(){
        return getBit(this->ADDRESS, this->REG_RF_MNGT_Dyn, 0);
    }

    void ST25DV::setRFDisableDyn(bool val){
        setBit(this->ADDRESS, this->REG_RF_MNGT_Dyn, 0, val);
    }

    bool ST25DV::getRFSleepDyn(){
        return getBit(this->ADDRESS, this->REG_RF_MNGT_Dyn, 1);
    }

    void ST25DV::setRFSleepDyn(bool val){
        setBit(this->ADDRESS, this->REG_RF_MNGT_Dyn, 1, val);
    }
    
    bool ST25DV::getI2CUnlocked(){
        return getBit(this->ADDRESS, this->REG_I2C_SSO_Dyn, 0);
    }

    uint8_t ST25DV::getInterruptSource(){
        return getByte(this->ADDRESS, this->REG_IT_STS_Dyn);
    }

    bool ST25DV::getFTMEnable(){
        return getBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 0);
    }

    void ST25DV::setFTMEnable(bool val){
        setBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 0, val);
    }

    uint8_t ST25DV::getMailboxStatus(){
        return getByte(this->ADDRESS, this->REG_MB_CTRL_Dyn);
    }

    bool ST25DV::getHostPutMessage(){
        return getBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 1);
    }

    bool ST25DV::getRFPutMessage(){
        return getBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 2);
    }

    bool ST25DV::getHostMissMessage(){
        return getBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 4);
    }

    bool ST25DV::getRFMissMessage(){
        return getBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 5);
    }

    bool ST25DV::getHostCurrentMessage(){
        return getBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 6);
    }

    bool ST25DV::getRFCurrentMessage(){
        return getBit(this->ADDRESS, this->REG_MB_CTRL_Dyn, 7);
    }

    uint8_t ST25DV::getMailboxMessageSize(){
        return getByte(this->ADDRESS, this->REG_MB_LEN_Dyn);
    }
    

//Fast transfer mode buffer functions
    /*uint8_t* ST25DV::readMailbox(){
        return getBulk(this->ADDRESS, this->REG_FAST_TRANSFER_START, getMailboxMessageSize());
    }

    void ST25DV::writeMailbox(uint8_t len, uint8_t* dat){
        setBulk(this->ADDRESS, this->REG_FAST_TRANSFER_START, len, dat);
    }*/




//System configuration area functions
    uint8_t ST25DV::getGPOMode(){
        return getByte(this->ADDRESS_CONFIG, this->REG_GPO) & 0x7F;
    }

    void ST25DV::setGPOMode(uint8_t mode){
        uint8_t buffer = getByte(this->ADDRESS_CONFIG, this->REG_GPO) & 0x80;
        buffer |= (mode & 0x7F);
        setByte(this->ADDRESS_CONFIG, this->REG_GPO, buffer);
    }

    bool ST25DV::getGPOEnabledBoot(){
        return getBit(this->ADDRESS_CONFIG, this->REG_GPO, 7);
    }

    void ST25DV::setGPOEnabledBoot(bool active){
        setBit(this->ADDRESS_CONFIG, this->REG_GPO, 7, active);
    }

    uint8_t ST25DV::getInterruptTime(){
        return getByte(this->ADDRESS_CONFIG, this->REG_IT_TIME);
    }

    void ST25DV::setInterruptTime(uint8_t tim){
        setByte(this->ADDRESS_CONFIG, this->REG_IT_TIME, tim & 0x07);
    }

    bool ST25DV::getEHForced(){
        return getBit(this->ADDRESS_CONFIG, this->REG_EH_MODE, 0);
    }

    void ST25DV::setEHForced(bool forced){
        setBit(this->ADDRESS_CONFIG, this->REG_EH_MODE, 0, forced);
    }

    bool ST25DV::getRFDisable(){
        return getBit(this->ADDRESS_CONFIG, this->REG_RF_MNGT, 0);
    }

    void ST25DV::setRFDisable(bool dis){
        setBit(this->ADDRESS_CONFIG, this->REG_RF_MNGT, 0, dis);
    }

    bool ST25DV::getRFSleep(){
        return getBit(this->ADDRESS_CONFIG, this->REG_RF_MNGT, 1);
    }

    void ST25DV::setRFSleep(bool slp){
        setBit(this->ADDRESS_CONFIG, this->REG_RF_MNGT, 1, slp);
    }

    uint8_t ST25DV::getENDA(uint8_t area){
        switch (area){
            case 1:
                return getByte(this->ADDRESS_CONFIG, this->REG_ENDA1);
            case 2:
                return getByte(this->ADDRESS_CONFIG, this->REG_ENDA2);
            case 3:
                return getByte(this->ADDRESS_CONFIG, this->REG_ENDA3);
            default:
                return 0;
        }
    }

    void ST25DV::setENDA(uint8_t area, uint8_t endpoint){
        switch (area){
        case 1:
            setByte(this->ADDRESS_CONFIG, this->REG_ENDA1, endpoint);
            break;
        case 2:
            setByte(this->ADDRESS_CONFIG, this->REG_ENDA2, endpoint);
            break;
        case 3:
            setByte(this->ADDRESS_CONFIG, this->REG_ENDA3, endpoint);
            break;
        default:
            break;
        }
    }

    uint8_t ST25DV::getRFZonePass(uint8_t area){
        switch (area){
            case 1:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA1SS) & 0x03;
            case 2:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA2SS) & 0x03;
            case 3:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA3SS) & 0x03;
            case 4:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA4SS) & 0x03;
            default:
                return 0;
        }
    }

    void ST25DV::setRFZonePass(uint8_t area, uint8_t pass){
        if((area > 0) && (area < 5)){
            uint16_t add = 0;
            switch(area){
                case 1:
                    add = this->REG_RFA1SS;
                    break;
                case 2:
                    add = this->REG_RFA2SS;
                    break;
                case 3:
                    add = this->REG_RFA3SS;
                    break;
                case 4:
                    add = this->REG_RFA4SS;
                    break;
                default:
                    break;
            }
            uint8_t buffer = getByte(this->ADDRESS_CONFIG, add) & 0xFC;
            buffer |= (pass & 0x03);
            setByte(this->ADDRESS_CONFIG, add, buffer);
        }
    }

    uint8_t ST25DV::getRFZoneLock(uint8_t area){
        switch (area){
            case 1:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA1SS) & 0x0C;
            case 2:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA2SS) & 0x0C;
            case 3:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA3SS) & 0x0C;
            case 4:
                return getByte(this->ADDRESS_CONFIG, this->REG_RFA4SS) & 0x0C;
            default:
                return 0;
        }
    }

    void ST25DV::setRFZoneLock(uint8_t area, uint8_t mode){
        if((area > 0) && (area < 5)){
            uint16_t add = 0;
            switch(area){
                case 1:
                    add = this->REG_RFA1SS;
                    break;
                case 2:
                    add = this->REG_RFA2SS;
                    break;
                case 3:
                    add = this->REG_RFA3SS;
                    break;
                case 4:
                    add = this->REG_RFA4SS;
                    break;
                default:
                    break;
            }
            uint8_t buffer = getByte(this->ADDRESS_CONFIG, add) & 0xF3;
            buffer |= ((mode & 0x03) << 2);
            setByte(this->ADDRESS_CONFIG, add, buffer);
        }
    }

    uint8_t ST25DV::getI2CZoneLock(uint8_t area){
        switch (area){
            case 1:
                return getByte(this->ADDRESS_CONFIG, this->REG_I2CSS) & 0x03;
            case 2:
                return getByte(this->ADDRESS_CONFIG, this->REG_I2CSS) & 0x0C;
            case 3:
                return getByte(this->ADDRESS_CONFIG, this->REG_I2CSS) & 0x30;
            case 4:
                return getByte(this->ADDRESS_CONFIG, this->REG_I2CSS) & 0xC0;
            default:
                return 0;
        }
    }

    void ST25DV::setI2CZoneLock(uint8_t area, uint8_t mode){
        uint8_t buffer = getByte(this->ADDRESS_CONFIG, this->REG_I2CSS);
        uint8_t newlock = mode & 0x03;
        switch (area){
            case 1:
                setByte(this->ADDRESS_CONFIG, this->REG_I2CSS, (buffer & 0xFC) | newlock);
                break;
            case 2:
                setByte(this->ADDRESS_CONFIG, this->REG_I2CSS, (buffer & 0xF3) | (newlock << 2));
                break;
            case 3:
                setByte(this->ADDRESS_CONFIG, this->REG_I2CSS, (buffer & 0xCF) | (newlock << 4));
                break;
            case 4:
                setByte(this->ADDRESS_CONFIG, this->REG_I2CSS, (buffer & 0x3F) | (newlock << 6));
                break;
            default:
                break;
        }
    }

    bool ST25DV::getCCFileLock(uint8_t block){
        return getBit(this->ADDRESS_CONFIG, this->REG_LOCK_CCFILE, block & 0x01);
    }

    void ST25DV::setCCFileLock(uint8_t block, bool val){
        setBit(this->ADDRESS_CONFIG, this->REG_LOCK_CCFILE, block * 0x01, val);
    }

    bool ST25DV::getMBEnabled(){
        return getBit(this->ADDRESS_CONFIG, this->REG_MB_MODE, 0);
    }

    void ST25DV::setMBEnabled(bool en){
        setBit(this->ADDRESS_CONFIG, this->REG_MB_MODE, 0, en);
    }

    uint8_t ST25DV::getMBTimeout(){
        return getByte(this->ADDRESS_CONFIG, this->REG_MB_WDG);
    }

    void ST25DV::setMBTimeout(uint8_t val){
        if(val & 0xF8){
            setByte(this->ADDRESS_CONFIG, this->REG_MB_WDG, 0x00);
        }
        else{
            setByte(this->ADDRESS_CONFIG, this-REG_MB_WDG, val);
        }
    }

    bool ST25DV::getRFCFGLock(){
        return getBit(this->ADDRESS_CONFIG, this->REG_LOCK_CFG, 0);
    }

    void ST25DV::setRFCFGLock(bool val){
        setBit(this->ADDRESS_CONFIG, this->REG_LOCK_CFG, 0, val);
    }

    uint8_t ST25DV::getDSFIDLock(){
        return getByte(this->ADDRESS_CONFIG, this->REG_LOCK_DSFID);
    }

    uint8_t ST25DV::getAFILock(){
        return getByte(this->ADDRESS_CONFIG, this->REG_LOCK_AFI);
    }

    uint8_t ST25DV::getDSFID(){
        return getByte(this->ADDRESS_CONFIG, this->REG_DSFID);
    }

    uint8_t ST25DV::getAFI(){
        return getByte(this->ADDRESS_CONFIG, this->REG_AFI);
    }

    uint16_t ST25DV::getMemBlocks(){
        return get16bits(this->ADDRESS_CONFIG, this->REG_MEM_SIZE_L);
    }

    uint8_t ST25DV::getBlockSize(){
        return getByte(this->ADDRESS_CONFIG, this->REG_BLK_SIZE);
    }

    uint8_t ST25DV::getSizeK(){
        switch(getByte(this->ADDRESS_CONFIG, this->REG_MEM_SIZE_H)){
            case 0x07:
                return 64;
            case 0x01:
                return 16;
            default:
                return 4;
        }
    }

    uint16_t ST25DV::getLastAdd(){
        switch (getSizeK()){
        case 04:
            return this->REG_USER_MEM_END_04K;
        case 16:
            return this->REG_USER_MEM_END_16K;
        case 64:
            return this->REG_USER_MEM_END_64K;
        }
    }

    uint8_t ST25DV::getICRef(){
        return getByte(this->ADDRESS_CONFIG, this->REG_IC_REF);
    }

    uint64_t ST25DV::getUID(){
        return get64bits(this->ADDRESS_CONFIG, this->REG_UID_START);
    }

    uint8_t ST25DV::getRevision(){
        return getByte(this->ADDRESS_CONFIG, this->REG_IC_REV);
    }

    uint64_t ST25DV::getI2CPass(){
        return get64bits(this->ADDRESS_CONFIG, this->REG_I2C_PWD_START);
    }

    void ST25DV::setI2CPass(uint64_t pass){
        set64bits(this->ADDRESS_CONFIG, this->REG_I2C_PWD_START, pass);
    }