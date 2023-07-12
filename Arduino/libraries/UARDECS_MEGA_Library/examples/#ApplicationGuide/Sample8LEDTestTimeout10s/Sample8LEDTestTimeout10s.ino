//Sample8LEDTestTimeout10s
#include <SPI.h>
#include <Ethernet2.h> //Arduino IDE Ver1.7.2以降でW5500搭載機種
//#include <Ethernet.h> //Ver1.7.2以降でW5100搭載機種
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Uardecs_mega.h>

const byte U_InitPin = 3;//このピンは変更可能です
const byte U_InitPin_Sense=HIGH;
const char U_name[] PROGMEM= "UARDECS Node v.1.0";
const char U_vender[] PROGMEM= "XXXXXX Co.";
const char U_uecsid[] PROGMEM= "000000000000";
const char U_footnote[] PROGMEM= "Test node";
char U_nodename[20] = "Sample";

UECSOriginalAttribute U_orgAttribute;
const int U_HtmlLine = 0;
struct UECSUserHtml U_html[U_HtmlLine]={};
const int U_MAX_CCM = 2;//CCMの総数を2に
UECSCCM U_ccmList[U_MAX_CCM];

//CCM定義用の素材1、被らないように適当な変数名で3つ宣言
const char ccmInfoCnd[] PROGMEM= "NodeCondition";//CCMの説明(Webでのみ表示)
const char ccmTypeCnd[] PROGMEM= "cnd.xXX";//CCMのType文字列
const char ccmUnitCnd[] PROGMEM= "";//CCMの単位(この場合単位無し)

//CCM定義用の素材2、被らないように適当な変数名で3つ宣言
const char ccmInfoRcv[] PROGMEM= "Receive CCM";//CCMの説明(Webでのみ表示)
const char ccmTypeRcv[] PROGMEM= "Receive.xXX";//CCMのType文字列
const char ccmUnitRcv[] PROGMEM= "";//CCMの単位(この場合単位無し)

void UserInit(){
U_orgAttribute.mac[0] = 0x00;
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x00;
//UECSsetCCM(送受信の区分,通し番号,CCM説明,Type,単位,priority,少数桁数,送信頻度設定[A_1S_0で1秒間隔])
//送信CCM
UECSsetCCM(true,0,ccmInfoCnd,ccmTypeCnd,ccmUnitCnd,29,0,A_1S_0);
//受信CCM
UECSsetCCM(false,1,ccmInfoRcv,ccmTypeRcv,ccmUnitRcv,29,0,S_1S_0);
}
void OnWebFormRecieved(){}
void UserEverySecond()
{
if(U_ccmList[1].validity && U_ccmList[1].value)
  {
  U_ccmList[0].value=1;
  digitalWrite(2,HIGH);
  }
else if(U_ccmList[1].flagStimeRfirst && U_ccmList[1].recmillis>10000)//10sec
  {
  U_ccmList[0].value=0;
  digitalWrite(2,LOW);
  }
}

void UserEveryMinute(){}
void UserEveryLoop(){}
void loop()
{
UECSloop();
}
void setup()
{
pinMode(2,OUTPUT);
UECSsetup();
}

