/////////////////////////////////////////
//UARDECS Sample Program "DummyInAirTemp"
//UECS ccm "InAirTemp" sending test Ver1.3
//By H.kurosaki 2016/1/14
//////////////////////////////////////////
//[概要]
//Web上から任意の値を指定し、InAirTempとして出力します。


#include <SPI.h>
#include <Ethernet2.h> //Arduino IDE Ver1.7.2以降でW5500搭載機種
//#include <Ethernet.h> //Ver1.7.2以降でW5100搭載機種
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Uardecs_mega.h>

/////////////////////////////////////
//IP reset jupmer pin setting
//IPアドレスリセット用ジャンパーピン設定
/////////////////////////////////////

const byte U_InitPin = 3;
const byte U_InitPin_Sense=HIGH;  

////////////////////////////////////
//Node basic infomation
///////////////////////////////////
const char U_name[] PROGMEM= "UARDECS Node v.1.1";//MAX 20 chars
const char U_vender[] PROGMEM= "XXXXXXXX Co.";//MAX 20 chars
const char U_uecsid[] PROGMEM= "000000000000";//12 chars fixed
const char U_footnote[] PROGMEM= "UARDECS Sample Program DummyInAirTemp";
//const int U_footnoteLetterNumber = 48;//Abolished after Ver 0.6
char U_nodename[20] = "Sample";//MAX 19chars
UECSOriginalAttribute U_orgAttribute;
//////////////////////////////////
// html page1 setting
//////////////////////////////////
const int U_HtmlLine = 1; //Total number of HTML table rows.
const char NAME2[] PROGMEM= "SetTemp";

const char CENT[] PROGMEM= "C";
const char NONES[] PROGMEM= "";
const char note1[] PROGMEM= "Set output temp";

signed long setTemp;
const char** dummy = NULL;

struct UECSUserHtml U_html[U_HtmlLine]={
{NAME2, UECSINPUTDATA, CENT, note1, dummy, 0, &(setTemp), -100, 1000, 1},
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
//MAC address is printed on sticker of Ethernet Shield.
//You must assign unique MAC address to each nodes.
//MACアドレス設定、必ずEthernet Shieldに書かれた値を入力して下さい。
//全てのノードに異なるMACアドレスを設定する必要があります。
U_orgAttribute.mac[0] = 0x00;
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x01;

//Set ccm list
UECSsetCCM(true, CCMID_InAirTemp, ccmNameTemp, ccmTypeTemp, ccmUnitTemp, 29, 1, A_1S_0);
UECSsetCCM(true,  CCMID_cnd      , ccmNameCnd , ccmTypeCnd , ccmUnitCnd , 29, 0, A_10S_0);
}


void OnWebFormRecieved(){
  U_ccmList[CCMID_InAirTemp].value=setTemp;
}

void UserEverySecond(){}
void UserEveryMinute(){}
void UserEveryLoop(){}

void loop(){
UECSloop();
}


void setup(){
UECSsetup();
U_ccmList[CCMID_InAirTemp].value=setTemp;
}


