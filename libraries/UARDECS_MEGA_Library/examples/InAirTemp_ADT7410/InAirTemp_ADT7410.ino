/////////////////////////////////////////
//UARDECS Sample Program for ADT7410
//Ver1.2
//By H.kurosaki 2015/12/10
//////////////////////////////////////////
//[概要]
//温度センサADT7410から温度を読み出してCCMに出力します
//[使い方]
//想定する動作環境はAruduinoUNO + Ethernet Shield2を標準としています
//MEGAでも動きますが、ピン番号の違いに注意して下さい
//○ArduinoIDE
//Ver1.7.2以降を使用します
//○使用するライブラリ
//UARDECS
//http://uecs.org/arduino/uardecs.html
//説明書に従ってインストールしてください
//
//○ADT7410の接続
//センサ→Arduino
//VDD→5V
//SCL→SCL(UNOではA5、MEGAではD20も可)
//SDA→SDA(UNOではA4、MEGAではD21も可)
//GND→GND
//
//○その他
//D3ピンがIPアドレスのリセット用に使用されています(変更可)



#include <SPI.h>
#include <Ethernet2.h> //Arduino IDE Ver1.7.2以降でW5500搭載機種
//#include <Ethernet.h> //Ver1.7.2以降でW5100搭載機種
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Uardecs_mega.h>

////////////////////////////ADT7410
//ADT7410のI2Cアドレスを設定する(設定方法はデータシート参照)
#define ADT7410ADDR 0x48
//センサのエラー時に発信する値
#define ERROR_SENSORSTOP 0x4000000

/////////////////////////////////////
//IP reset jupmer pin setting
/////////////////////////////////////
//Pin ID. This pin is pull-upped automatically.
const byte U_InitPin = 3;//このピンは変更可能です
const byte U_InitPin_Sense=HIGH;  
////////////////////////////////////
//Node basic infomation
///////////////////////////////////
const char U_name[] PROGMEM= "UARDECS Node v.1.0";
const char U_vender[] PROGMEM= "XXXXXX Co.";
const char U_uecsid[] PROGMEM= "000000000000";
const char U_footnote[] PROGMEM= "Test node";
char U_nodename[20] = "Sample";
UECSOriginalAttribute U_orgAttribute;
//////////////////////////////////
// html page1 setting
//////////////////////////////////
const int U_HtmlLine = 0; //Total number of HTML table rows.
struct UECSUserHtml U_html[U_HtmlLine]={
};

//////////////////////////////////
// UserCCM setting
//////////////////////////////////
//define CCMID for identify
//CCMID_dummy must put on last
enum {
CCMID_InAirTemp,
CCMID_cnd,
CCMID_dummy,
};

const int U_MAX_CCM = CCMID_dummy;
UECSCCM U_ccmList[U_MAX_CCM];

const char ccmNameTemp[] PROGMEM= "Temperature";
const char ccmTypeTemp[] PROGMEM= "InAirTemp";
const char ccmUnitTemp[] PROGMEM= "C";

const char ccmNameCnd[] PROGMEM= "NodeCondition";
const char ccmTypeCnd[] PROGMEM= "cnd.mIC";
const char ccmUnitCnd[] PROGMEM= "";

void UserInit(){
//必ずシールドに貼ってあるアドレスに書き換えて使用して下さい
U_orgAttribute.mac[0] = 0x00;
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x02;

//Set ccm list
UECSsetCCM(true, CCMID_InAirTemp, ccmNameTemp, ccmTypeTemp, ccmUnitTemp, 29, 1, A_1S_0);
UECSsetCCM(true,  CCMID_cnd      , ccmNameCnd , ccmTypeCnd , ccmUnitCnd , 29, 0, A_10S_0);
}
void OnWebFormRecieved(){
}
void UserEverySecond(){
  readADT7410();
}
void UserEveryMinute(){
}
void UserEveryLoop(){
}
void loop(){
UECSloop();
}
void setup(){
UECSsetup();
Wire.begin(); 
  
}

void readADT7410(){

  unsigned short readval;
  int tempx16;
 
  if(Wire.requestFrom(ADT7410ADDR, 2)==0)
    {
    //センサへのアクセス失敗
    U_ccmList[CCMID_cnd].value=ERROR_SENSORSTOP;
    return;
    }
    
  readval = (unsigned short)Wire.read() << 8;   // データ読み出し1
  readval |= Wire.read();                 // データ読み出し2
  readval >>= 3;                          // ビットシフト
  tempx16 = (int)readval;                 // 整数化
 
  if(readval & (0x8000 >> 3)) {         //正負判定
    tempx16 = tempx16  - 8192;          //マイナスの時
  }
 
  float temperature;
  temperature = (float)tempx16 / 16.0;       // ℃換算

  //仕様上U_ccmList[*].valueには整数値しか格納できません
  //Set ccm listで小数点に1を指定した場合、1の位が少数第1位として扱われます
  //temperatureは浮動小数点型なので10倍した後整数値に変換して対応します
  U_ccmList[CCMID_InAirTemp].value=(long)(temperature*10);
  
  //正常時にはcnd=0
  U_ccmList[CCMID_cnd].value=0;
}


