//Sample5AutoRange
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
//CCM定義用の素材、被らないように適当な変数名で3つ宣言(必ずPROGMEMを付ける)
const char ccmInfoTest[] PROGMEM= "NodeCondition";//CCMの説明(Webでのみ表示)
const char ccmTypeTest[] PROGMEM= "cnd.mOC";//CCMのType文字列
const char ccmUnitTest[] PROGMEM= "";//CCMの単位(この場合単位無し)

const char ccmInfoRad[] PROGMEM= "Radiation";//CCMの説明(Webでのみ表示)
const char ccmTypeRad[] PROGMEM= "WRadiation";//CCMのType文字列(屋外日射)
const char ccmUnitRad[] PROGMEM= "kW m-2";//CCMの単位

void UserInit(){
U_orgAttribute.mac[0] = 0x00;
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x00;

//UECSsetCCM(送受信の区分[trueで送信],通し番号[0から始まる],CCM説明,Type,単位,priority[通常は29],少数桁数,送信頻度設定[A_1S_0で1秒間隔])
UECSsetCCM(true,0,ccmInfoTest,ccmTypeTest,ccmUnitTest,29,0,A_1S_0);
UECSsetCCM(true,1,ccmInfoRad,ccmTypeRad,ccmUnitRad,29,3,A_1S_0);
}

void OnWebFormRecieved(){}

void UserEverySecond()
{
static bool lowRange=false;
double radiation=analogRead(0);
if(radiation==1023 && lowRange)
  {
//1.1Vモードでレンジ外になった場合、5Vモードに変更
    lowRange=false;
    analogReference(DEFAULT);//UNO基準電圧5V
    analogRead(0);//読み飛ばす
    return;
  }

if(radiation<200 && !lowRange)
  {
//5Vモードで1V未満になった場合、1.1Vモードに変更
    lowRange=true;

#if defined(INTERNAL1V1)
    analogReference(INTERNAL1V1);//MEGA基準電圧1.1V
#else
    analogReference(INTERNAL);//UNO基準電圧1.1V
#endif

    analogRead(0);//読み飛ばす
    return;
  }

if(lowRange)
  {
  //1.1Vモード換算式
  radiation= radiation*1.0/930.0;
  }
 else
  {
  //5Vモード換算式
  radiation= radiation*1.0/204.6;
  }
  U_ccmList[1].value= radiation*1000; //少数桁が3なので1000倍
  U_ccmList[0].value= lowRange;//現在の計測レンジを出力
}

void UserEveryMinute(){}
void UserEveryLoop(){}

void loop()
{
UECSloop();
}

void setup()
{
analogReference(DEFAULT);//UNO基準電圧5V
UECSsetup();
}

