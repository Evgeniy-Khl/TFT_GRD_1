#include "tft_proc.h"
#include "ili9341.h"
#include "ds18b20.h"
#include "displ.h"
#include "rtc.h"
#include "my.h"

extern char buffTFT[];
extern const char* setName[];
extern const char* modeName[];
extern uint8_t displ_num, mode, ds18b20_amount, ds18b20_num, familycode[][8], newButt, Y_txt, X_left, Y_top, Y_bottom, newDate, status;
extern int16_t ds18b20_val[], fillScreen, set[], newval[];
extern int8_t numSet, numDate;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

extern int8_t relaySet[8];
extern int8_t analogSet[2];
extern int8_t relOut[8], analogOut[4];

int16_t min(int16_t a, int16_t b ) {
   return a < b ? a : b;
}

int16_t max(int16_t a, int16_t b ) {
   return a > b ? a : b;
}

//--------- �������� ����� ----------------------
void displ_0(void){
  Y_txt = Y_top; X_left = 5; 
  if(newButt){
    newButt = 0;
    ILI9341_FillRectangle(0, Y_txt, ILI9341_WIDTH, ILI9341_HEIGHT, fillScreen);
    initializeButtons(3,1,45);// 3 �������; ���� ������; ������ 45
    if(status) drawButton(ILI9341_MAGENTA, 0, "����");
    else drawButton(ILI9341_GREEN, 0, "����");
    drawButton(ILI9341_YELLOW, 1, "�������.");
    drawButton(ILI9341_CYAN, 2, "�������.");
  }
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  if(status) ILI9341_WriteString(X_left+30, Y_txt, " ON ", Font_11x18, ILI9341_BLACK, ILI9341_GREEN);
  else ILI9341_WriteString(X_left+30, Y_txt, " OFF ", Font_11x18, ILI9341_YELLOW, ILI9341_RED);
  
  ILI9341_WriteString(X_left+110, Y_txt, "�����:", Font_11x18, ILI9341_YELLOW, fillScreen);
  sprintf(buffTFT,"%8s", modeName[mode]);
  ILI9341_WriteString(X_left+180, Y_txt, buffTFT, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);
  Y_txt = Y_txt+18+15;
  ILI9341_WriteString(X_left+50, Y_txt, "����������� � �����I", Font_11x18, ILI9341_YELLOW, fillScreen);
  Y_txt = Y_txt+18+5;
  
  if(ds18b20_val[0]<1000) sprintf(buffTFT," %3.1f ",(float)ds18b20_val[0]/10);
  else if(ds18b20_val[0]<1270) sprintf(buffTFT," %3d  ", ds18b20_val[0]/10);
  else strcat(buffTFT," ***  ");
  ILI9341_WriteString(X_left+60, Y_txt, buffTFT, Font_11x18, ILI9341_BLACK, ILI9341_CYAN);
  sprintf(buffTFT," %3i,0", set[0]);
  ILI9341_WriteString(X_left+180, Y_txt, buffTFT, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);
  Y_txt = Y_txt+18+5;
  ILI9341_WriteString(X_left+40, Y_txt, "����������� � �������I", Font_11x18, ILI9341_YELLOW, fillScreen);
  Y_txt = Y_txt+18+5;
  if(ds18b20_val[1]<1000) sprintf(buffTFT," %3.1f ",(float)ds18b20_val[1]/10);
  else if(ds18b20_val[1]<1270) sprintf(buffTFT," %3d  ", ds18b20_val[1]/10);
  else strcat(buffTFT," ***  ");
  ILI9341_WriteString(X_left+60, Y_txt, buffTFT, Font_11x18, ILI9341_BLACK, ILI9341_CYAN);
  sprintf(buffTFT," %3i,0", set[1]);
  ILI9341_WriteString(X_left+180, Y_txt, buffTFT, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);
  Y_txt = Y_txt+18+5;
  ILI9341_WriteString(X_left+60, Y_txt, "������I��� ������", Font_11x18, ILI9341_YELLOW, fillScreen);
  Y_txt = Y_txt+18+5;
  if(status){
    sprintf(buffTFT," %02u:%02u:%02u ", sTime.Hours, sTime.Minutes, sTime.Seconds);
    ILI9341_WriteString(X_left+30, Y_txt, buffTFT, Font_11x18, ILI9341_YELLOW, fillScreen);
  }
  sprintf(buffTFT," %i���.%02i���.", set[3]/60, set[3]%60);
  ILI9341_WriteString(X_left+160, Y_txt, buffTFT, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);
}

//-------------------------------- ���� ����Ĳ� ------------------------------------------------------
void displ_1(void){
 uint8_t i;
 char txt[10];
 uint16_t color_txt, color_box; 
    Y_txt = Y_top;
    if(newButt){
      newButt = 0;// keystop=1; 
      ILI9341_FillRectangle(0, Y_txt, ILI9341_WIDTH, ILI9341_HEIGHT, fillScreen);
      ILI9341_WriteString(X_left+60, Y_txt,"���� �������",Font_11x18,ILI9341_YELLOW,fillScreen);
      initializeButtons(4,1,25);// ������ �������; ���� ������; ������ 25
      drawButton(ILI9341_BLUE, 0, "�����");
      drawButton(ILI9341_YELLOW, 1, "�����");
      drawButton(ILI9341_MAGENTA, 2, "+");
      drawButton(ILI9341_CYAN, 3, "-");
    }
//---- �������� ������ ----
    Y_txt = Y_txt+18+5;
    for (i=0;i<6;i++){
        sprintf(buffTFT,"����  N%u: ",i+1);
        if(relaySet[i]<0) strcat(buffTFT,"AUTO"); else if(relaySet[i]==1) strcat(buffTFT," ON "); else strcat(buffTFT," OFF");
        if(i == numSet){color_txt = ILI9341_BLACK; color_box = ILI9341_WHITE;} else {color_txt = ILI9341_WHITE; color_box = ILI9341_BLACK;}
        ILI9341_WriteString(X_left+10, Y_txt, buffTFT, Font_11x18, color_txt, color_box);
        if(relOut[i]) color_box=ILI9341_YELLOW; else color_box=ILI9341_COLOR565(128, 128, 128);
        ILI9341_FillRectangle(X_left+200,Y_txt,20,18,color_box);
        Y_txt = Y_txt+18+5;
    }
//    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** �������� ����� ������ ***************************************
//---- ���������� ������ ----
    for (i=0;i<2;i++){
        sprintf(buffTFT,"����� N%u: ",i+1);
      if(analogSet[i]<0) strcat(buffTFT,"AUTO"); else {strcat(buffTFT,"SET:"); analogOut[i]=analogSet[i];}
        sprintf(txt," %3u%% ",analogOut[i]);
        strcat(buffTFT,txt);
        if(i+6 == numSet){color_txt = ILI9341_BLACK; color_box = ILI9341_WHITE;} else {color_txt = ILI9341_WHITE; color_box = ILI9341_BLACK;}
        ILI9341_WriteString(X_left+10,Y_txt, buffTFT, Font_11x18, color_txt, color_box);
        Y_txt = Y_txt+18+5;    
    }
//    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** �������� ����� ������ ***************************************
}

//--------- ������������ ----------------------------------
void displ_2(void){
  int8_t i;
  uint16_t color_txt, color_box;

  Y_txt = Y_top; X_left = 5;
  if (newButt){
    newButt = 0;
    ILI9341_FillRectangle(0, Y_txt, ILI9341_WIDTH, ILI9341_HEIGHT, fillScreen);
    initializeButtons(4,1,45);// ������ �������; ���� ������; ������ 45
    drawButton(ILI9341_BLUE, 0, "�����");
    drawButton(ILI9341_GREEN, 1, "v");
    drawButton(ILI9341_GREEN, 2, "^");
    drawButton(ILI9341_YELLOW, 3, "�����");
  }
  Y_txt = Y_txt+10;
  for (i=-1; i<MAX_SET; i++){
    if(i==-1) sprintf(buffTFT,"       �����: %8s", modeName[mode]);
    else if(i==3) sprintf(buffTFT,"%12s: %i���.%02i���.", setName[i], set[i]/60, set[i]%60);
    else {
      sprintf(buffTFT,"%12s: %3i", setName[i], set[i]);
      switch (i){
      	case 4: strcat(buffTFT,"���.");	break;
      	case 5: strcat(buffTFT,"%");	break;
      	default: strcat(buffTFT,"���."); break;
      }
    }
    if(i == numSet){color_txt = ILI9341_BLACK; color_box = ILI9341_WHITE;} else {color_txt = ILI9341_WHITE; color_box = ILI9341_BLACK;}
    ILI9341_WriteString(X_left, Y_txt, buffTFT, Font_11x18, color_txt, color_box);
    Y_txt = Y_txt+18+5;
  }
}

//--------- �̲�� ���������� ----------------------------------
void displ_3(void){
  Y_txt = Y_top; X_left = 5;
  if (newButt){
    newButt = 0;
    ILI9341_FillRectangle(0, Y_txt, ILI9341_WIDTH, ILI9341_HEIGHT, fillScreen);
    initializeButtons(4,1,45);// ������ �������; ���� ������; ������ 45
    drawButton(ILI9341_BLUE, 0, "���.");
    drawButton(ILI9341_GREEN, 1, "+");
    drawButton(ILI9341_GREEN, 2, "-");
    drawButton(ILI9341_MAGENTA, 3, "���.");
  }
  Y_txt = Y_txt+50;
  sprintf(buffTFT,"%12s:", setName[numSet]);
  ILI9341_WriteString(X_left+20, Y_txt, buffTFT, Font_11x18, ILI9341_WHITE, ILI9341_BLACK);

  if(numSet==3) sprintf(buffTFT,"%i���.%02i���.", newval[numSet]/60, newval[numSet]%60);
  else sprintf(buffTFT,"%3i", newval[numSet]);
  if(numSet==3) ILI9341_WriteString(X_left+180, Y_txt, buffTFT, Font_11x18, ILI9341_WHITE, ILI9341_BLACK);
  else {
    Y_txt = Y_txt-4;
    ILI9341_WriteString(X_left+180, Y_txt, buffTFT, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);
  }
}

//--------- �̲�� ������ ----------------------------------
void displ_4(void){
  uint8_t i;
  uint16_t color_txt, color_box;
  Y_txt = Y_top; X_left = 5;
  if (newButt){
    newButt = 0;
    ILI9341_FillRectangle(0, Y_txt, ILI9341_WIDTH, ILI9341_HEIGHT, fillScreen);
    initializeButtons(4,1,45);// ������ �������; ���� ������; ������ 45
    drawButton(ILI9341_BLUE, 0, "�����");
    drawButton(ILI9341_GREEN, 1, "v");
    drawButton(ILI9341_GREEN, 2, "^");
    drawButton(ILI9341_YELLOW, 3, "�����");
  }
  Y_txt = Y_txt+10;
  for (i=0; i<MAX_MODE; i++){
    sprintf(buffTFT,"%10s", modeName[i]);
    if(i == newval[0]){color_txt = ILI9341_BLACK; color_box = ILI9341_WHITE;} else {color_txt = ILI9341_WHITE; color_box = ILI9341_BLACK;}
    ILI9341_WriteString(X_left, Y_txt, buffTFT, Font_11x18, color_txt, color_box);
    Y_txt = Y_txt+18+5;
  }
}

void display(void){
  switch (displ_num){
  	case 0: displ_0(); break;//- ���� ������ --
  	case 1: displ_1(); break;//- ���� ����Ĳ� -
    case 2: displ_2(); break;//- ������������ -
    case 3: displ_3(); break;//- �̲�� ���������� -
    case 4: displ_4(); break;//- �̲�� ������ -
  	default: displ_0();	break;//- ���� ������ -
  }
}
