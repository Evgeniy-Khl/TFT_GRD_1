#include "main.h"
#include "my.h"
#include "nvRam.h"

#define MAGIC_KEY_DEFINE 0x19630301
#define FLASH_CONFIG_START_ADDR ((uint32_t) 0x0801FC00) // ����� ������ � 126 �������� �� ������ 0x0801F800
#define FLASH_CONFIG_END_ADDR   FLASH_CONFIG_START_ADDR + FLASH_PAGE_SIZE

extern uint8_t mode;
extern int16_t set[MAX_SET];
extern CRC_HandleTypeDef hcrc;

union DataRam dataRAM;

uint32_t calcChecksum(void){
    __HAL_RCC_CRC_CLK_ENABLE(); // ��������� clock ��� CRC ������
//    CRC->CR = CRC_CR_RESET;     // ���������� CRC ������
    HAL_CRC_Init(&hcrc);
    // ��������� ����� ��� ���������� CRC
    uint32_t* configPtr = (uint32_t*)&dataRAM.config.magicNum;
    // ��������� CRC ��� ���� ����� ��������� Config, ����� ����� notUsed[5], ���� checkSum � ���� countSave
    uint8_t sizeBuff = (sizeof(struct Config) - sizeof(uint32_t)*6 - sizeof(uint16_t)) / sizeof(uint32_t);
    uint32_t CRCVal = HAL_CRC_Calculate(&hcrc, configPtr, sizeBuff);
    
    __HAL_RCC_CRC_CLK_DISABLE();// ��������� clock ��� CRC ������
    return CRCVal;// ���������� �������� CRC
}

void setData(uint8_t m){
  switch (m){
  	case 0: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet0[i];} break;
  	case 1: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet1[i];} break;
    case 2: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet2[i];} break;
  	case 3: for(uint8_t i=0;i<MAX_SET;i++){set[i] = dataRAM.config.modeSet3[i];} break;
  	default: mode = dataRAM.config.mode = 0; for(uint8_t i=0;i<5;i++){set[i] = dataRAM.config.modeSet0[i];}	break;
  }
}

uint8_t initData(void){
  // ������ ������ �� FLASH ������
  uint8_t err=0;
  uint32_t l_Address = FLASH_CONFIG_START_ADDR;
  uint32_t l_End = FLASH_CONFIG_START_ADDR + sizeof(struct Config);
  uint32_t l_Index = 0x00;
  
  while (l_Address < l_End){    // FLASH_CONFIG_END_ADDR
    // ((__IO uint32_t *)l_Address)
    // ���������� ������ l_Address � ��������� �� 32-������ (4-�������) ����������� ����� ����� � ��������� __IO, 
    // ������� ����������, ��� ��� "����/�����" (Input/Output) ����������.
    // *(...) - ������������� ����� ���������, ����� �������� �������� �� ����� ������.
    dataRAM.data32[l_Index] = *(__IO uint32_t *)l_Address;
    l_Index = l_Index + 1;
    l_Address = l_Address + 4;
  }
//  for(uint8_t i=0;i<5;i++){dataRAM.config.notUsed[i]=0;}
  // ���� CRC �� ��������� ...
  l_Index = calcChecksum();
  if(l_Index != dataRAM.config.checkSum) err |= 2;
  
  // ���� ����� ������ ���� �� ������� �� ��� ������ ������
  if(dataRAM.config.magicNum != MAGIC_KEY_DEFINE || err){
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
    err |= 1;
    for(uint8_t i=0;i<5;i++){dataRAM.config.notUsed[i]=0;}
    
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
    dataRAM.config.modeSet3[5]=100;// %
    
    for(uint8_t i=0;i<8;i++){dataRAM.config.relaySet[i]=-1;}
    for(uint8_t i=0;i<2;i++){dataRAM.config.analogSet[i]=-1;}
    
    dataRAM.config.checkSum = calcChecksum();
    dataRAM.config.countSave = 0;
  }
  mode = dataRAM.config.mode;
  setData(mode);
  return err;
}

uint32_t writeData(void){
  static FLASH_EraseInitTypeDef EraseInitStruct;
  // ��������� ������ ��� ������� ������
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_CONFIG_START_ADDR;
  EraseInitStruct.NbPages     = 0x01;

  uint32_t l_Address = FLASH_CONFIG_START_ADDR;
  uint32_t l_End = FLASH_CONFIG_START_ADDR + sizeof(struct Config);
  uint32_t l_Error = 0x00;
  uint32_t l_Index = 0x00;
  // ��������� �����
  dataRAM.config.mode = mode;
  switch (mode){
  	case 0: for(uint8_t i=0;i<MAX_SET;i++){dataRAM.config.modeSet0[i] = set[i];} break;
  	case 1: for(uint8_t i=0;i<MAX_SET;i++){dataRAM.config.modeSet1[i] = set[i];} break;
    case 2: for(uint8_t i=0;i<MAX_SET;i++){dataRAM.config.modeSet2[i] = set[i];} break;
  	case 3: for(uint8_t i=0;i<MAX_SET;i++){dataRAM.config.modeSet3[i] = set[i];} break;
  	default: dataRAM.config.mode=mode=0;  for(uint8_t i=0;i<5;i++){dataRAM.config.modeSet0[i] = set[i];}	break;
  }
  dataRAM.config.checkSum = calcChecksum();
  dataRAM.config.countSave = dataRAM.config.countSave + 1;
  // ��������� � ������ � ������
  HAL_FLASH_Unlock();
  // ������� �������� ������
  HAL_FLASHEx_Erase(&EraseInitStruct, &l_Error);
  // ����� ������ � ������
  while (l_Address < l_End){   //  FLASH_CONFIG_END_ADDR
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, l_Address, dataRAM.data32[l_Index]) == HAL_OK){
      l_Index = l_Index + 1;
      l_Address = l_Address + 4;
    }
  }
  // ��������� ������ �� ������
  HAL_FLASH_Lock();
  
  // ��������� ����������� ������
  l_Address = FLASH_CONFIG_START_ADDR;
  l_Error = 0x00;
  l_Index = 0x00; // 5 ����� �������� ����������� �� ����������� �������!!
  while (l_Address < l_End){
    if(dataRAM.data32[l_Index] != *(__IO uint32_t *)l_Address){
      if(l_Index>4) l_Error++;
    }
    l_Index = l_Index + 1;
    l_Address = l_Address + 4;
  }
  return l_Error;
}
/*
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
}*/
