/////////////////////////////////////////
//UARDECS Sample Program "CO2 sensor S300"
//I2C connection test with S300 Ver1.1
//By H.kurosaki 2017/03/01
//////////////////////////////////////////

//ELT sensor社のCO2センサ S300シリーズからI2C通信によりCO2濃度値を読み出します。
//アナログ値は電源電圧の誤差によって精度が低下しますが、この方法では精度低下がありません。
//温室用にはS300Gを選択するのが良いと思われます。
//Arduino UNOまたはMEGAに直結する場合、5Vモデルを使用する必要があります。
//3.3Vモデルを使用する場合は
//http://akizukidenshi.com/catalog/g/gM-05452/
//などを使ってセンサ側のバス電圧を3.3Vにする必要があります。


//結線方法 以下の４ピンを使用します
//[Arduino]-[S300]
//[SCL]-[SCL]
//[SDA]-[SDA]
//[5V]-[+5V]
//[GND]-[GND]

//センサを校正する場合
//S300のMCDLというピンをArduinoのGNDに直結して電源を入れたまま10分間400ppmのCO2に晒します

#include <SPI.h>
#include <Ethernet2.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Uardecs_mega.h>
#include <Wire.h>


#define OPRMODE_ERR_S300SENSERR        0x10000000 //0 00100 00000 0 0000 0000000000000000 //センサ異常

char S300_sensSts;
unsigned char S300_errcount=0;


/////////////////////////////////////
//IP reset jupmer pin setting
//IPアドレスリセット用ジャンパーピン設定
/////////////////////////////////////
const byte U_InitPin = 3;
const byte U_InitPin_Sense=HIGH;


////////////////////////////////////
//Node basic infomation
//ノードの基本情報
///////////////////////////////////
const char U_name[] PROGMEM= "CO2 Node";//MAX 20 chars
const char U_vender[] PROGMEM= "WARC/NARO";//MAX 20 chars
const char U_uecsid[] PROGMEM= "000000000000";//12 chars fixed
const char U_footnote[] PROGMEM= "CO2 Sensor Node";
char U_nodename[20] = "node";//MAX 19chars (This value enabled in safemode)
UECSOriginalAttribute U_orgAttribute;//この定義は弄らないで下さい
//////////////////////////////////
// html page1 setting
//Web上の設定画面に関する宣言
//////////////////////////////////

//Total number of HTML table rows.
//web設定画面で表示すべき項目の総数
const int U_HtmlLine = 0;

//表示素材の登録
struct UECSUserHtml U_html[U_HtmlLine]={
};

//////////////////////////////////
// UserCCM setting
// CCM用の素材
//////////////////////////////////
enum {
CCMID_InAirCO2,
CCMID_cnd,
CCMID_dummy, //CCMID_dummyは必ず最後に置くこと
};


//CCM格納変数の宣言
//ここはこのままにして下さい
const int U_MAX_CCM = CCMID_dummy;
UECSCCM U_ccmList[U_MAX_CCM];

//CCM定義用の素材
const char ccmNameCO2[] PROGMEM= "CO2";
const char ccmTypeCO2[] PROGMEM= "InAirCO2";
const char ccmUnitCO2[] PROGMEM= "ppm";

const char ccmNameCnd[] PROGMEM= "NodeCondition";
const char ccmTypeCnd[] PROGMEM= "cnd.mIC";
const char ccmUnitCnd[] PROGMEM= "";

//------------------------------------------------------
void UserInit(){
U_orgAttribute.mac[0] = 0x00;
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x01;

UECSsetCCM(true, CCMID_InAirCO2, ccmNameCO2, ccmTypeCO2, ccmUnitCO2, 29, 0, A_10S_0);
UECSsetCCM(true, CCMID_cnd      , ccmNameCnd , ccmTypeCnd , ccmUnitCnd , 29,0, A_10S_0);
}
//---------------------------------------------------------
void OnWebFormRecieved(){
}
//---------------------------------------------------------
void UserEverySecond(){
  U_ccmList[CCMID_cnd].value=0;
   if (S300_errcount>3)
        {
          S300_errcount=4;
         U_ccmList[CCMID_cnd].value|=OPRMODE_ERR_S300SENSERR;
        }


//------------------------------------------------------CO2計測
  U_ccmList[CCMID_InAirCO2].value=(long)S300_readCO2();
  if(S300_sensSts)
  {
  S300_errcount++;
  }
  else
  {
    S300_errcount=0;
    }

//エラー時にCCM送信を停止する
if(S300_errcount)
  {
        U_ccmList[CCMID_InAirCO2].flagStimeRfirst=false;
    }


}
//---------------------------------------------------------
void UserEveryMinute(){
}
//---------------------------------------------------------
void UserEveryLoop(){
}

//---------------------------------
void loop(){
UECSloop();
}

//---------------------------------
void setup(){

Wire.begin();

UECSsetup();
}

//----------------------------------------S300
//----------------------------------------
//----------------------------------------
#define  eS300Address   0x31
#define  eS300Read   0x52
uint16_t S300_readCO2(void)
{
  //S300 addr 
Wire.begin(eS300Address);
Wire.beginTransmission(eS300Address);
Wire.write(eS300Read);//read command
S300_sensSts=Wire.endTransmission();
if(S300_sensSts!=0)
  {return 0;}//err
Wire.requestFrom(eS300Address, 7);
if(Wire.available()==7)//err check
  {
  byte config=Wire.read();
  delay(1);
  byte co2h=Wire.read();
  delay(1);
  byte co2l=Wire.read();
  delay(1);
  /*実装不要
  Wire.read();//dummy data
  delay(1);
  Wire.read();//dummy data
  delay(1);
  Wire.read();//dummy data
  delay(1);
  Wire.read();//dummy data
  delay(1);*/
  S300_sensSts=0;
  return ((uint16_t)co2h)*256+co2l;
  }
  
  S300_sensSts=4;
  return 0;
  }




