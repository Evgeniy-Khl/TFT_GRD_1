#include "main.h"
#include "my.h"
#include "nvRam.h"

#define MAGIC_KEY_DEFINE 0x19630301
#define FLASH_CONFIG_START_ADDR ((uint32_t) 0x0801FC00) // Будем писать в 126 страницу по адресу 0x0801F800
#define FLASH_CONFIG_END_ADDR   FLASH_CONFIG_START_ADDR + FLASH_PAGE_SIZE

extern uint8_t mode;
extern int16_t set[MAX_SET];
extern CRC_HandleTypeDef hcrc;

union DataRam dataRAM;

uint32_t calcChecksum(void){
    __HAL_RCC_CRC_CLK_ENABLE(); // Включение clock для CRC модуля
//    CRC->CR = CRC_CR_RESET;     // Сбрасываем CRC модуль
    HAL_CRC_Init(&hcrc);
    // Начальное слово для вычисления CRC
    uint32_t* configPtr = (uint32_t*)&dataRAM.config.magicNum;
    // Вычисляем CRC для всех полей структуры Config, кроме полей notUsed[5], поля checkSum и поля countSave
    uint8_t sizeBuff = (sizeof(struct Config) - sizeof(uint32_t)*6 - sizeof(uint16_t)) / sizeof(uint32_t);
    uint32_t CRCVal = HAL_CRC_Calculate(&hcrc, configPtr, sizeBuff);
    
    __HAL_RCC_CRC_CLK_DISABLE();// Отключаем clock для CRC модуля
    return CRCVal;// Возвращаем значение CRC
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
  // Чтение данных из FLASH памяти
  uint8_t err=0;
  uint32_t l_Address = FLASH_CONFIG_START_ADDR;
  uint32_t l_End = FLASH_CONFIG_START_ADDR + sizeof(struct Config);
  uint32_t l_Index = 0x00;
  
  while (l_Address < l_End){    // FLASH_CONFIG_END_ADDR
    // ((__IO uint32_t *)l_Address)
    // приведение адреса l_Address к указателю на 32-битное (4-байтное) беззнаковое целое число с атрибутом __IO, 
    // который обозначает, что это "вход/выход" (Input/Output) переменная.
    // *(...) - разыменование этого указателя, чтобы получить значение по этому адресу.
    dataRAM.data32[l_Index] = *(__IO uint32_t *)l_Address;
    l_Index = l_Index + 1;
    l_Address = l_Address + 4;
  }
//  for(uint8_t i=0;i<5;i++){dataRAM.config.notUsed[i]=0;}
  // Если CRC не совпадают ...
  l_Index = calcChecksum();
  if(l_Index != dataRAM.config.checkSum) err |= 2;
  
  // Если после чтения ключ не найденб то это первый запуск
  if(dataRAM.config.magicNum != MAGIC_KEY_DEFINE || err){
  /*
  //---------------- spT1,spT2,spT3,spTmr,coolTmr,
  uint16_t modeSet0[4]={70,  60,  50,  180,  60}  // сек.
  uint16_t modeSet1[4]={71,  61,  51,  181,  61}  // грд, грд, грд, мин, сек.
  uint16_t modeSet2[4]={72,  62,  52,  182,  62}  // грд, грд, грд, мин, сек.
  uint16_t modeSet3[4]={73,  63,  53,  183,  63}  // грд, грд, грд, мин, сек.
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
    dataRAM.config.modeSet0[0]=70;// грд
    dataRAM.config.modeSet0[1]=60;// грд
    dataRAM.config.modeSet0[2]=50;// грд
    dataRAM.config.modeSet0[3]=180;// мин
    dataRAM.config.modeSet0[4]=160;// сек
    dataRAM.config.modeSet0[5]=30;// %
    
    dataRAM.config.modeSet1[0]=71;// грд
    dataRAM.config.modeSet1[1]=61;// грд
    dataRAM.config.modeSet1[2]=51;// грд
    dataRAM.config.modeSet1[3]=181;// мин
    dataRAM.config.modeSet1[4]=161;// сек
    dataRAM.config.modeSet1[5]=50;// %
    
    dataRAM.config.modeSet2[0]=72;// грд
    dataRAM.config.modeSet2[1]=62;// грд
    dataRAM.config.modeSet2[2]=52;// грд
    dataRAM.config.modeSet2[3]=182;// мин
    dataRAM.config.modeSet2[4]=162;// сек
    dataRAM.config.modeSet2[5]=70;// %
    
    dataRAM.config.modeSet3[0]=73;// грд
    dataRAM.config.modeSet3[1]=63;// грд
    dataRAM.config.modeSet3[2]=53;// грд
    dataRAM.config.modeSet3[3]=183;// мин
    dataRAM.config.modeSet3[4]=163;// сек
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
  // Структура данных для очистки памяти
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_CONFIG_START_ADDR;
  EraseInitStruct.NbPages     = 0x01;

  uint32_t l_Address = FLASH_CONFIG_START_ADDR;
  uint32_t l_End = FLASH_CONFIG_START_ADDR + sizeof(struct Config);
  uint32_t l_Error = 0x00;
  uint32_t l_Index = 0x00;
  // Заполняем масив
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
  // Готовимся к записи в память
  HAL_FLASH_Unlock();
  // Очищаем страницу памяти
  HAL_FLASHEx_Erase(&EraseInitStruct, &l_Error);
  // Пишем данные в память
  while (l_Address < l_End){   //  FLASH_CONFIG_END_ADDR
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, l_Address, dataRAM.data32[l_Index]) == HAL_OK){
      l_Index = l_Index + 1;
      l_Address = l_Address + 4;
    }
  }
  // Закрываем доступ на запись
  HAL_FLASH_Lock();
  
  // Проверяем соотвествие данных
  l_Address = FLASH_CONFIG_START_ADDR;
  l_Error = 0x00;
  l_Index = 0x00; // 5 полей портятся процессором по неизвестной причине!!
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
  // Структура данных для очистки памяти
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_CONFIG_START_ADDR;
  EraseInitStruct.NbPages     = 0x01;
  
  // Проверяем соотвествие данных
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
    // Готовимся к записи в память
    HAL_FLASH_Unlock();
    // Очищаем страницу памяти
    HAL_FLASHEx_Erase(&EraseInitStruct, &l_Error);
    // Пишем данные в память
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
    // Закрываем доступ на запись
    HAL_FLASH_Lock();
  }
}*/
