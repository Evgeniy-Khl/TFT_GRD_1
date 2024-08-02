#include "main.h"

#define MAGIC_KEY_DEFINE 0x19630301
#define FLASH_CONFIG_START_ADDR ((uint32_t) 0x0801F800) // ����� ������ � 126 �������� �� ������ 0x0801F800
#define FLASH_CONFIG_END_ADDR   FLASH_CONFIG_START_ADDR + FLASH_PAGE_SIZE

typedef struct{
  uint32_t magicNum; //0x12345678
  uint8_t relayBitMask[3];
  uint8_t relayDelay[3];
  uint8_t relayInv[3];
  uint32_t countSave;
} Config_t;

struct FLASH_sector{
  uint8_t data[2048-8];
  uint32_t countSave;
  uint32_t checkSum;
};

union {
  Config_t config;
  struct FLASH_sector sector;
  uint32_t data32[512];
} dataRAM;

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

    dataRAM.config.relayBitMask[0]=0x70;
    dataRAM.config.relayBitMask[1]=0x0c;
    dataRAM.config.relayBitMask[2]=0x03;
    
    dataRAM.config.relayDelay[0]=50;
    dataRAM.config.relayDelay[1]=100;
    dataRAM.config.relayDelay[2]=150;
    
    dataRAM.config.relayInv[0]=0;
    dataRAM.config.relayInv[1]=0;
    dataRAM.config.relayInv[0]=0;
    
    dataRAM.config.magicNum = MAGIC_KEY_DEFINE;
    dataRAM.config.countSave = 0;
  }
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
    dataRAM.sector.countSave = dataRAM.sector.countSave + 1;
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
