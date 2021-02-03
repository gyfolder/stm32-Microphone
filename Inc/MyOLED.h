
#define OLED_Comm       0
#define OLED_Data       1

#define OLED_Char6X8    0
#define OLED_Char8X16   1
#define OLED_Num3X5     2

void SetRESPin(void* gpio, unsigned char pin);
void SetDCPin(void* gpio, unsigned char pin);
void SetSSPin(void* gpio, unsigned char pin);
void Setpinstate(int _0res_1dc_2ss,char set);
void sendSPI(unsigned char Indata, unsigned char DC);
void send2SPI(unsigned char Indata1, unsigned char Indata2, unsigned char DC);
void ShowBuffer();
void ClearBuffer(unsigned char DC);
void OLEDinit();
int CharToNum(char inchar);
void ShowDrawDot(unsigned char x, unsigned char y, unsigned char OnOff);
void ShowDrawChar(unsigned char x, unsigned char y, char Fonts, char charater);
void ShowDrawString(unsigned char x, unsigned char y, char Fonts, char* Strings);
void NoShowDrawDot(unsigned char x, unsigned char y, unsigned char OnOff);
void NoShowDrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char OnOff);
void NoShowDrawNum(unsigned char x, unsigned char y, char Fonts, int Num, int EngTrueBigFalseSmall);
void NoShowDrawNum1(unsigned char x, unsigned char y, char Fonts, int Num, unsigned char OnOff, int EngTrueBigFalseSmall);
void NoShowDrawChar(unsigned char x, unsigned char y, char Fonts, char charater);
void NoShowDrawChar1(unsigned char x, unsigned char y, char Fonts, char charater, unsigned char OnOff);
void NoShowDrawString(unsigned char x, unsigned char y, char Fonts, char* Strings);
void NoShowDrawString1(unsigned char x, unsigned char y, char Fonts, char* Strings, unsigned char OnOff);
void NoShowDrawArea(unsigned char x, unsigned char y, unsigned char x1, unsigned char y1, unsigned char OnOff);
void NoShowDrawAreaW(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char OnOff);
void NoShowDrawNumStr(unsigned char x, unsigned char y, char Fonts, char* Strings, int EngTrueBigFalseSmall);
void NoShowDrawNumStr1(unsigned char x, unsigned char y, char Fonts, char* Strings, unsigned char OnOff, int EngTrueBigFalseSmall);
void NoShowDrawCircle(unsigned char x, unsigned char y, unsigned char r, unsigned char OnOff, unsigned char Full);
void NoShowDrawPicture(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char* Picture, unsigned char OnOff);