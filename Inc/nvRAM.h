#ifndef _NVRAM_H
#define _NVRAM_H

#include <stdint.h>

struct Config {
    uint32_t notUsed[5];// эти 5 полей портятся процессором по неизвестной причине!!
    uint32_t magicNum;
    uint16_t mode;
    uint16_t modeSet0[MAX_SET];
    uint16_t modeSet1[MAX_SET];
    uint16_t modeSet2[MAX_SET];
    uint16_t modeSet3[MAX_SET];
    int8_t relaySet[8];
    int8_t analogSet[2];
    uint32_t checkSum;
    uint32_t countSave;
};

union DataRam {
    struct Config config;
    uint32_t data32[256];
};

extern union DataRam dataRAM;

uint8_t initData(void);
uint32_t calcChecksum(void);
uint32_t writeData(void);

#endif /* _NVRAM_H */
