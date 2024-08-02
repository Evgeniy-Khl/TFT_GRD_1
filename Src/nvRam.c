#include "main.h"
#include "my.h"

#define MAGIC_KEY_DEFINE 0x19630301
#define FLASH_CONFIG_START_ADDR ((uint32_t) 0x0801F800) // ����� ������ � 126 �������� �� ������ 0x0801F800
#define FLASH_CONFIG_END_ADDR   FLASH_CONFIG_START_ADDR + FLASH_PAGE_SIZE

extern uint8_t mode;
extern int16_t set[MAX_SET];

struct Config{
  uint32_t magicNum; //0x19630301
  uint8_t mode;
  uint16_t modeSet0[MAX_SET];
  uint16_t modeSet1[MAX_SET];
  uint16_t modeSet2[MAX_SET];
  uint16_t modeSet3[MAX_SET];
  int8_t relaySet[8];
  int8_t analogSet[2];
  uint16_t checkSum;
  uint16_t countSave;
};

union {
  struct Config config;
  uint32_t data32[512];
} dataRAM;

void setData(uint8_t m){
  switch (m){
  	case 0: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet0[i];} break;
  	case 1: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet1[i];} break;
    case 2: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet2[i];} break;
  	case 3: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet3[i];} break;
  	default: mode = dataRAM.config.mode = 0; for(uint8_t i=0;i<5;i++){set[i] = dataRAM.config.modeSet0[i];}	break;
  }
}

void initData(void){
  // ������ ������ �� FLASH ������
  uint32_t l_Address = FLASH_CONFIG_START_ADDR;
  uint32_t l_Index = 0x00;
  
  while (l_Address < FLASH_CONFIG_END_ADDR){
    // ((__IO uint32_t *)l_Address)
    // ���������� ������ l_Address � ��������� �� 32-������ (4-�������) ����������� ����� ����� � ��������� __IO, 
    // ������� ����������, ��� ��� "����/�����" (Input/Output) ����������.
    // *(...) - ������������� ����� ���������, ����� �������� �������� �� ����� ������.
    dataRAM.data32[l_Index] = *(__IO uint32_t *)l_Address;
    l_Index = l_Index + 1;
    l_Address = l_Address + 4;
  }
  
  // ���� ����� ������ ���� �� ������� �� ��� ������ ������
  if(dataRAM.config.magicNum != MAGIC_KEY_DEFINE){
  /*
  //---------------- spT1,spT2,spT3,spTmr,coolTmr,
  uint16_t modeSet0[4]={70,  60,  50,  180,  60}  // ���.
  uint16_t modeSet1[4]={71,  61,  51,  181,  61}  // ���, ���, ���, ���, ���.
  uint16_t modeSet2[4]={72,  62,  52,  182,  62}  // ���, ���, ���, ���, ���.
  uint16_t modeSet3[4]={73,  63,  53,  183,  63}  // ���, ���, ���, ���, ���.
  //------------------auto
  int8_t relaySet[8]={-1,-1,-1,-1,-1,-1,-1,-1};
  //------------------auto
  int8_t analogSet[2]={-1,-1};
  */  
    dataRAM.config.magicNum=MAGIC_KEY_DEFINE;
    
    dataRAM.config.mode = 0;
    //- spT1,spT2,spT3,spTmr,coolTmr,
    dataRAM.config.modeSet0[0]=70;// ���
    dataRAM.config.modeSet0[1]=60;// ���
    dataRAM.config.modeSet0[2]=50;// ���
    dataRAM.config.modeSet0[3]=180;// ���
    dataRAM.config.modeSet0[4]=160;// ���
    dataRAM.config.modeSet0[5]=30;// %
    
    dataRAM.config.modeSet1[0]=71;// ���
    dataRAM.config.modeSet1[1]=61;// ���
    dataRAM.config.modeSet1[2]=51;// ���
    dataRAM.config.modeSet1[3]=181;// ���
    dataRAM.config.modeSet1[4]=161;// ���
    dataRAM.config.modeSet1[5]=50;// %
    
    dataRAM.config.modeSet2[0]=72;// ���
    dataRAM.config.modeSet2[1]=62;// ���
    dataRAM.config.modeSet2[2]=52;// ���
    dataRAM.config.modeSet2[3]=182;// ���
    dataRAM.config.modeSet2[4]=162;// ���
    dataRAM.config.modeSet2[5]=70;// %
    
    dataRAM.config.modeSet3[0]=73;// ���
    dataRAM.config.modeSet3[1]=63;// ���
    dataRAM.config.modeSet3[2]=53;// ���
    dataRAM.config.modeSet3[3]=183;// ���
    dataRAM.config.modeSet3[4]=163;// ���
    dataRAM.config.modeSet2[5]=100;// %
    
    for(uint8_t i=0;i<8;i++){dataRAM.config.relaySet[i]=-1;}
    for(uint8_t i=0;i<2;i++){dataRAM.config.analogSet[i]=-1;}
    
    dataRAM.config.checkSum = 0;
    dataRAM.config.countSave = 0;
  }
  mode = dataRAM.config.mode;
  setData(mode);
}

void checkData(void){
  
  static FLASH_EraseInitTypeDef EraseInitStruct;
  // ��������� ������ ��� ������� ������
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_CONFIG_START_ADDR;
  EraseInitStruct.NbPages     = 0x01;
  
  // ��������� ����������� ������
  uint32_t l_Address = FLASH_CONFIG_START_ADDR;
  uint32_t l_Error = 0x00;
  uint32_t l_Index = 0x00;
  while (l_Address < FLASH_CONFIG_END_ADDR){
    if(dataRAM.data32[l_Index] != *(__IO uint32_t *)l_Address){
      l_Error++;
    }
    l_Index = l_Index + 1;
    l_Address = l_Address + 4;
  }
  if(l_Error){
    // ��������� � ������ � ������
    HAL_FLASH_Unlock();
    // ������� �������� ������
    HAL_FLASHEx_Erase(&EraseInitStruct, &l_Error);
    // ����� ������ � ������
    l_Address = FLASH_CONFIG_START_ADDR;
    l_Error = 0x00;
    l_Index = 0x00;
    dataRAM.config.countSave = dataRAM.config.countSave + 1;
    while (l_Address < FLASH_CONFIG_END_ADDR){     
      if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, l_Address, dataRAM.data32[l_Index]) == HAL_OK){
        l_Index = l_Index + 1;
        l_Address = l_Address + 4;
      }
    }
    // ��������� ������ �� ������
    HAL_FLASH_Lock();
  }
}
