/***************************************************************************
 * �� �������� ���������� ��� ������ � RTC STM32F1xx �� �������� UnixTime*
 * �������� ������������� � ��������� � ������� (����, ����, ��)		   *
 * � ��� (������, �������, �������), � �������							   *
 ***************************************************************************/
#include "main.h"
#define SECOND_A_DAY 86400

uint32_t colodarSetting(int year, char month, char day, char hour, char min, char sec);
//void counterToColodar (unsigned long counter, unixColodar * unixTime);
unsigned long colodarToCounter (void);
