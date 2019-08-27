/*
 * ST25DV.h
 *
 *  Created on: August 27, 2019
 *      Author: Max Robinson
 */



#ifndef ST25DV_h
#define ST25DV_h

#include "arduino.h"
#include "ST25DV.h"
#include Wire.h
#include stdint.h

class ST25DV
{
    public:
    ST25DV(Wire port, uint8_t gpoPin);//TODO: Inherit Wire instance gracefully
    ST25DV(Wire port);
    
    
    
    //Worker functions
        //TODO get and set bulk functions
        uint8_t getByte(uint8_t add, uint16_t reg);
        void setByte(uint8_t add, uint16_t reg, uint8_t dat);
        bool getBit(uint8_t add, uint8_t reg, uint8_t bit);
        void setBit(uint8_t add, uint8_t reg, uint8_t bit, bool dat);



    //User memory functions




    //Dynamic register functions
        uint8_t getGPOMode_Dyn();
        void setGPOMode_Dyn(uint8_t val);
        uint8_t getGPOEnabled_Dyn();
        void setGPOEnabled_Dyn(uint8_t val);

        uint8_t getEHEnabled();
        void setEHEnabled(uint8_t val);
        uint8_t getEHActive();//TODO: Check meaning
        uint8_t getRFFieldPresent();
        uint8_t getVCCOn();

        uint8_t getRFDisable_Dyn();
        void setRFDisable_Dyn(uint8_t val);
        uint8_t getRFSleep_Dyn();
        void setRFSleep_Dyn(uint8_t val);

        uint8_t getI2CUnlocked();

        uint8_t getInterruptSource();

        uint8_t getFTMEnable();
        void setFTMEnable(uint8_t val);
        uint8_t getHostPutMessage();
        uint8_t getRFPutMessage();
        uint8_t getHostMissMessage();
        uint8_t getRFMissMessage();
        uint8_t getHostCurrentMessage();
        uint8_t getRFCurrentMessage();

        uint8_t getMailboxMessageSize();



    //Fast transfer mode buffer functions
        




    //System configuration area functions
        uint8_t unlockI2C(uint64_t pass);
        void lockI2C();
        
        uint8_t changeI2CPassword(uint64_t newpass);
        
        
        //TODO add rest of registers
        uint8_t getENDA(uint8_t area);
        void setENDA(uint8_t area, uint8_t endpoint);
        //TODO add rest of registers


        uint8_t getRFDisable();
        void setRFDisable(uint8_t val);
        uint8_t getRFSleep();
        void setRFSleep(uint8_t val);



        uint8_t getDSFIDLock();
        uint8_t getAFILock();
        uint8_t getDSFID();
        uint8_t getAFI();
        uint16_t getMemBlocks();
        uint8_t getBlockSize();
        uint16_t getSizeK();
        uint8_t getICRef();//TODO: Check purpose and verify name
        uint64_t getUID();
        uint8_t getRevision();
        uint64_t getI2CPass();
        void setI2CPass(uint64_t pass);
        



    private:
        Wire WIREPORT;//Inherit properly
        uint8_t GPO_PIN;
        uint8_t sizeK;
        uint8_t FTM_ENABLED;
        
        const uint8_t ADDRESS = 0xA6;//or its 0x53(if last bit is not direction)//For user memory, dynamic registers, FTM mailbox
        const uint8_t ADDRESS_CONFIG = 0xAE;//or its 0x57(if last bit is not direction)//For sytem config registers

    //User memory registers
        const uint16_t REG_USER_MEM_START = 0x0000;
        const uint16_t REG_USER_MEM_END_04K = 0x01FF;
        const uint16_t REG_USER_MEM_EXT_MODE_REQ = 0x0400;
        const uint16_t REG_USER_MEM_END_16K = 0x07FF;
        const uint16_t REG_USER_MEM_END_64K = 0x1FFF;


    //Dynamic registers
        const uint16_t REG_GPO_CTRL_Dyn = 0x2000;
        const uint16_t REG_EH_CONTROL_Dyn = 0x2002;
        const uint16_t REG_RF_MNGT_Dyn = 0x2003;
        const uint16_t REG_I2C_SSO_Dyn = 0x2004;
        const uint16_t REG_IT_STS_Dyn = 0x2005;
        const uint16_t REG_MB_CTRL_Dyn = 0x2006;
        const uint16_t REG_MB_LEN_Dyn = 0x2007;


    //Fast transfer registers
        const uint16_t REG_FAST_TRANSFER_START = 0x2008;//Inclusive
        const uint16_t REG_FAST_TRANSFER_END = 0x2107;//Inclusive


    //System config registers
        const uint16_t REG_GPO = 0x0000;
        const uint16_t REG_IT_TIME = 0x0001;
        const uint16_t REG_EH_MODE = 0x0002;
        const uint16_t REG_RF_MNGT = 0x0003;
        const uint16_t REG_RFA1SS = 0x0004;
        const uint16_t REG_ENDA1 = 0x0005;
        const uint16_t REG_RFA2SS = 0x0006;
        const uint16_t REG_ENDA2 = 0x0007;
        const uint16_t REG_RFA3SS = 0x0008;
        const uint16_t REG_ENDA3 = 0x0009;
        const uint16_t REG_RFA4SS = 0x000A;
        const uint16_t REG_I2CSS = 0x000B;
        const uint16_t REG_LOCK_CCFILE = 0x000C;
        const uint16_t REG_MB_MODE = 0x000D;
        const uint16_t REG_MB_WDG = 0x000E;
        const uint16_t REG_LOCK_CFG = 0x000F;
        const uint16_t REG_LOCK_DSFID = 0x0010;
        const uint16_t REG_LOCK_AFI = 0x0011;
        const uint16_t REG_DSFID = 0x0012;
        const uint16_t REG_AFI = 0x0013;
        const uint16_t REG_MEM_SIZE_L = 0x0014;
        const uint16_t REG_MEM_SIZE_H = 0x0015;
        const uint16_t REG_BLK_SIZE = 0x0016;
        const uint16_t REG_IC_REF = 0x0017;
        const uint16_t REG_UID_START = 0x0018;
        const uint16_t REG_IC_REV = 0x0020;
        const uint16_t REG_I2C_PWD_START = 0x0900;
};
#endif