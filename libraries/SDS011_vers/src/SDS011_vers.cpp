//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// SDS011_vers.cpp       dust sensor PM2.5 and PM10
// -------------------------------------------------------------------------------------
//
// idea based on SDS011 By R. Zschiegner (rz@madavi.de)
// extension based on
// Documentation:
// The iNovaFitness Laser Dust Sensor Control Protocol V1.4 SDS011
//
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

#include "SDS011_vers.h"

//--------------------------------------------------------------------------------------
//   checksum query = sum databytes 1-15 / 1-7
//--------------------------------------------------------------------------------------

SDS011_vers::SDS011_vers(void) {    }  // constructor

//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// begin
// -------------------------------------------------------------------------------------
void SDS011_vers::begin(uint8_t pin_rx, uint8_t pin_tx) {
  _pin_rx = pin_rx;
  _pin_tx = pin_tx;

  SoftwareSerial *softSerial = new SoftwareSerial(_pin_rx, _pin_tx);

  softSerial->begin(9600);

  sds_data = softSerial;
}

//--------------------------------------------------------------------------------------
/*
  void SDS011_vers::begin(HardwareSerial* serial) {
  serial->begin(9600);
  sds_data = serial;
  } */
//--------------------------------------------------------------------------------------

void SDS011_vers::begin(SoftwareSerial* serial) {
  serial->begin(9600);

  sds_data = serial;
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// 2  begin
//--------------------------------------------------------------------------------------
//  int SetDataReportingMode(queryset,activequery,DevID1c,DevID2c,querysetr,acticequeryr,DevID1r,DevID2r)
//      still effective after power off
//--------------------------------------------------------------------------------------

static byte data_reporting_mode_query_SDS_cmd[19] = {


  0xAA, // head
  0xB4, // command id
  0x02, // data byte 1
  0x01, // data byte 2 ( 0 query mode) (1 set mode)
  0x01, // data byte 3 ( 0 report active mode DEFAULT)    (1 report query mode)
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1) FF= all
  0xFF, // data byte 15 (device id byte 2) FF= all
  0x02, // checksum
  0xAB  // tail
};

//checksum = summe databytes 1-15 = 2+1+1+ff+ff= 2

//-------------------------------------------------------------------------------------------------------

int SDS011_vers::SetDataReportingMode(int *queryset, int *activequery, int *DevID1c, int *DevID2c, int *querysetr, int *activequeryr, int *DevID1r, int *DevID2r)
{


#ifdef DEBUG2
    Serial.println(" ");

    Serial.println("_Begin Queryactive Main Function_");
    Serial.print("queryset "); Serial.print(*queryset, HEX); Serial.print("  activequery "); Serial.println(*activequery, HEX);
    Serial.print("DevID1c "); Serial.print(*DevID1c, HEX); Serial.print("  DevID2c "); Serial.println (*DevID2c, HEX);
  #endif

  data_reporting_mode_query_SDS_cmd[3] = *queryset;
  data_reporting_mode_query_SDS_cmd[4] = *activequery;
  data_reporting_mode_query_SDS_cmd[15] = *DevID1c;
  data_reporting_mode_query_SDS_cmd[16] = *DevID2c;

#ifdef DEBUG2
    Serial.print("queryset      ");  Serial.println(data_reporting_mode_query_SDS_cmd[3]);
    Serial.print("activequery   ");  Serial.println(data_reporting_mode_query_SDS_cmd[4]);
  #endif


  int checksum_is = 0;
  checksum_is = data_reporting_mode_query_SDS_cmd[2] + data_reporting_mode_query_SDS_cmd[3] + data_reporting_mode_query_SDS_cmd[4] +   data_reporting_mode_query_SDS_cmd[15] + data_reporting_mode_query_SDS_cmd[16];
  data_reporting_mode_query_SDS_cmd[17] = checksum_is % 256;

#ifdef DEBUG2
    Serial.print("checksum new  ");  Serial.println(data_reporting_mode_query_SDS_cmd[17]);
    Serial.print("Checksum_is % 256:  "); Serial.println(checksum_is % 256);
  #endif


  // write buffer
  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(data_reporting_mode_query_SDS_cmd[i]);
  }
  sds_data->flush();  // write buffer to sensor


#ifdef DEBUG2
    Serial.println("buffer written");
    Serial.println("__________________________________________________________________");
  #endif

  //------------------------------------


  // read answer
  int antwort[9];
  byte buffer;
  int value;
  //  int len = 0;
  checksum_is = 0;
  int checksum_ok = 0;
  int error = 1;
  bool DataOK = false;
  int z = 0;

  //-------------------------

  //  Serial.println("warte auf Data Available 2");
  while (!sds_data->available() or z < 9 ) {
    delay(150); // Warten bis Daten da sind
#ifdef DEBUG2
      Serial.print(".");                                               // Problem endlos weil sich sensor ncach cmd abschaltet ???
    #endif

    z++; if (z >= 9) {
      break;
    };
  }
  //Serial.println();

  if (  (sds_data->available() > 0) && (sds_data->available() >= 10 ) )
  {

    //Serial.println(" vordataok ");
    while (DataOK == false) {

      // Serial.println(" indataok ");
      buffer = sds_data->read();
      value = int(buffer);

      // Serial.print("Value:  "); Serial.println(value, HEX);

      if (value = 0xAA )
      {
        for (uint8_t z = 1; z < 10; z++) {
          buffer = sds_data->read();
          antwort[z] = int(buffer);
#ifdef DEBUG2
 
            Serial.print(" 0x");          Serial.println(antwort[z], HEX);
          #endif
        }
      }

      //Serial.println("__________________________________________________________________");

      if (antwort[1] == 0xC5 && antwort[2] == 2 && antwort[9] == 0xAB)
      {
        checksum_is = antwort[2] + antwort[3] + antwort[4] + antwort[5] + antwort[6] + antwort[7];

        //Serial.print(" Ver2 all hex antwort checksum_is checksum_is % 256 ");
        //Serial.print(antwort[8], HEX); Serial.print(" = ");Serial.print(checksum_is, HEX);Serial.print(" = ");Serial.println(checksum_is % 256, HEX);

        if (antwort[8] == checksum_is % 256)
        {
          *querysetr  = antwort[3];
          *activequeryr = antwort[4];

          *DevID1r = antwort[6];
          *DevID2r = antwort[7];
          DataOK = true;
          error = 0;
        } else {
          for (uint8_t z = 1; z < 10; z++) {
            antwort[z] = 0;
          }
        }
      }
    }  // end while dataok
  }


  //--------------------------
  yield();

#ifdef DEBUG2
    //  Serial.println("________________________________________________________________--");
    Serial.print(" querysetr "); Serial.print(*querysetr, HEX); Serial.print("  activequeryr "); Serial.print(*activequeryr, HEX);
    Serial.print(" DevID1 "); Serial.print(*DevID1r, HEX); Serial.print("  DevID2 "); Serial.println (*DevID2r, HEX);
    //  Serial.println("_End of Queryactive Function_");
    //  Serial.println("__________________________________________________________________");
  #endif

  return error;
}  // END SDS011_vers::Setdatareportingmode

//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
//  int SetActiveReportingMode
//  auf active mode setzen     (Factory default is active reporting)
//--------------------------------------------------------------------------------------
int  SDS011_vers::SetActiveReportingMode() {
  int queryset = 1;
  int activequery = 0;
  int DevID1c = 0xFF;
  int DevID2c = 0xFF;
  int _qs;
  int _ar ;
  int _res;
  int _DevB1r ;
  int _DevB2r ;
  int _error = SDS011_vers::SetDataReportingMode(&queryset, &activequery, &DevID1c, &DevID2c, &_qs, &_ar, &_DevB1r, &_DevB2r);
  return _error;
}




//--------------------------------------------------------------------------------
//  int SetQUERYReportingMode
//  auf QUERY mode setzen     (Factory default is active reporting)
//--------------------------------------------------------------------------------
int   SDS011_vers::SetQueryReportingMode() {
  int queryset = 1;
  int activequery = 1;
  int DevID1c = 0xFF;
  int DevID2c = 0xFF;

  int _qs = 0;
  int _ar = 0;
  int _res = 0;
  int _DevB1r = 0;
  int _DevB2r = 0;
  int _error = SDS011_vers::SetDataReportingMode(&queryset, &activequery, &DevID1c, &DevID2c, &_qs, &_ar, &_DevB1r, &_DevB2r);
  return _error;
}


//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// 2  end
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// 4  begin
// -------------------------------------------------------------------------------------
// SDS011_vers::QueryDataCommand
// -------------------------------------------------------------------------------------


static byte QueryDataCMD[19] = {
  0xAA, // head
  0xB4, // command id
  0x04, // data byte 1
  0x00, // data byte 2
  0x00, // data byte 3
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1) FF= all
  0xFF, // data byte 15 (device id byte 2) FF= all
  0x02, // checksum
  0xAB  // tail
};

//checksum = summe databytes 1-15 = 4+0+0+ff+ff= 2    //



//--------------------------------------------------------------------------------------
int SDS011_vers::QueryDataCommand(int *DevID1c, int *DevID2c, float *p25, float *p10, int *DevID1r, int *DevID2r)
{

#ifdef DEBUG4
    Serial.println(" ");

    Serial.println("_Begin QueryDataCommand Main Function_");
    Serial.print("DevID1c "); Serial.print(*DevID1c, HEX); Serial.print("DevID2c "); Serial.println (*DevID2c, HEX);
    // Serial.print("p25 "); Serial.print(*p25, HEX); Serial.print("p10 "); Serial.println(*p10, HEX);
    // Serial.print("DevID1r "); Serial.print(*DevID1r, HEX); Serial.print("DevID2r "); Serial.println (*DevID2r, HEX);
  #endif

  QueryDataCMD[15] = *DevID1c;
  QueryDataCMD[16] = *DevID2c;

#ifdef DEBUG4
    Serial.print("DevID1c     ");  Serial.println(QueryDataCMD[15], HEX);
    Serial.print("DevID2c     ");  Serial.println(QueryDataCMD[16], HEX);
  #endif


  int checksum_is = 0;
  checksum_is = QueryDataCMD[2] + QueryDataCMD[3] + QueryDataCMD[4] + QueryDataCMD[15] + QueryDataCMD[16];
  QueryDataCMD[17] = checksum_is % 256;

#ifdef DEBUG4
    Serial.print("checksum new ");  Serial.println(QueryDataCMD[17]);
    Serial.print("Checksum_is % 256:  "); Serial.println(checksum_is % 256);
  #endif


  // write buffer
  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(QueryDataCMD[i]);
  }
  sds_data->flush();  // write buffer to sensor


#ifdef DEBUG4
    Serial.println("buffer written");
    // Serial.println("__________________________________________________________________");
  #endif

  //------------------------------------


  // read answer
  int antwort[9];
  byte buffer;
  int value;
  //  int len = 0;
  checksum_is = 0;
  int checksum_ok = 0;
  int error = 1;
  bool DataOK = false;
  int z = 0;

  //-------------------------

  //  Serial.println("warte auf Data Available 2");
  while (!sds_data->available() or z < 9 ) {
    delay(150); // Warten bis Daten da sind

#ifdef DEBUG4
      Serial.print("-");
    #endif

    z++; if (z >= 9) {
      break;
    };
  }
  Serial.println();

  if (  (sds_data->available() > 0) && (sds_data->available() >= 10 ) )
  {

    //Serial.println(" vordataok ");
    while (DataOK == false) {

      // Serial.println(" indataok ");
      buffer = sds_data->read();
      value = int(buffer);

      // Serial.print("Value:  "); Serial.println(value, HEX);

      if (value = 0xAA )
      {
        for (uint8_t z = 1; z < 10; z++) {
          buffer = sds_data->read();
          antwort[z] = int(buffer);

#ifdef DEBUG4
     
            Serial.print(" 0x");          Serial.println(antwort[z], HEX);
          #endif
        }
      }

      //Serial.println("__________________________________________________________________");

      if (antwort[1] == 0xC0 && antwort[9] == 0xAB)
      {
        checksum_is = antwort[2] + antwort[3] + antwort[4] + antwort[5] + antwort[6] + antwort[7];

       #ifdef DEBUG4
          //Serial.print(" Ver2 all hex antwort checksum_is checksum_is % 256 ");
          //Serial.print(antwort[8], HEX); Serial.print(" = ");Serial.print(checksum_is, HEX);Serial.print(" = ");Serial.println(checksum_is % 256, HEX);
          Serial.println();
          Serial.println(antwort[3], BIN); //high
          Serial.println();
          Serial.println(antwort[2], BIN); //low
          Serial.println();
          Serial.println(antwort[5], BIN); //high
          Serial.println();
          Serial.println(antwort[4], BIN); //low
          Serial.println();
        #endif

        if (antwort[8] == checksum_is % 256)
        {
          *p25  = (antwort[2] += (antwort[3] << 8)) / 10.0; // 2.5  Low  Byte += 2.5  High Byte
          *p10  = (antwort[4] += (antwort[5] << 8)) / 10.0; // 10   Low  Byte += 10   High Byte
          *DevID1r = antwort[6];
          *DevID2r = antwort[7];
          DataOK = true;
          error = 0;
        } else {
          for (uint8_t z = 1; z < 10; z++) {
            antwort[z] = 0;
          }
        }
      }
    }  // end while dataok
  }


  //--------------------------
  yield();

#ifdef DEBUG4
    Serial.println("________________________________________________________________--");
    Serial.print("DevID1c "); Serial.print(*DevID1c, HEX); Serial.print("DevID2c "); Serial.println (*DevID2c, HEX);
    Serial.print("p2.5 "); Serial.print(*p25, HEX); Serial.print("p10 "); Serial.println(*p10, HEX);
    Serial.print("DevID1r "); Serial.print(*DevID1r, HEX); Serial.print("DevID2r "); Serial.println (*DevID2r, HEX);
    Serial.println("_End of QueryDatacommand Function_");
    Serial.println("__________________________________________________________________");
  #endif
  return error;
}  // END SDS011_vers::querydatacommand



//--------------------------------------------------------------------------------------

// --------------------------------------------------------
// SDS011_vers:read_q   for query reporting mode
// --------------------------------------------------------
int SDS011_vers::read_q(float *p25, float *p10) {

  int DevID1c = 0xFF;
  int DevID2c = 0xFF;
  int _DevID1r = 0, _DevID2r = 0;
  int _error = SDS011_vers::QueryDataCommand(&DevID1c, &DevID2c, *&p25,  *&p10, &_DevID1r, &_DevID2r);


  return _error;
}

//--------------------------------------------------------------------------------------

// --------------------------------------------------------
// SDS011_vers:read_a   for  active reporting mode
// --------------------------------------------------------
int SDS011_vers::read_a(float *p25, float *p10) {
  byte buffer;
  int value;
  int len = 0;
  int pm10_serial = 0;
  int pm25_serial = 0;
  int checksum_is;
  int checksum_ok = 0;
  int error = 1;
  int z = 0;

  //-------------------------

  //  Serial.println("warte auf Data Available 2");
  while (!sds_data->available() or z < 9 ) {
    delay(150); // Warten bis Daten da sind
    Serial.print(".");                                               // Problem endlos weil sich sensor ncach cmd abschaltet ???
    z++; if (z >= 9) {
      break;
    };
  }


  while ((sds_data->available() > 0) && (sds_data->available() >= (10 - len))) {
    buffer = sds_data->read();
    value = int(buffer);
    switch (len) {
      case (0): if (value != 170) {
          len = -1;
        }; break;
      case (1): if (value != 192) {
          len = -1;
        }; break;
      case (2): pm25_serial = value; checksum_is = value; break;
      case (3): pm25_serial += (value << 8); checksum_is += value; break;
      case (4): pm10_serial = value; checksum_is += value; break;
      case (5): pm10_serial += (value << 8); checksum_is += value; break;
      case (6): checksum_is += value; break;
      case (7): checksum_is += value; break;
      case (8): if (value == (checksum_is % 256)) {
          checksum_ok = 1;
        } else {
          len = -1;
        }; break;
      case (9): if (value != 171) {
          len = -1;
        }; break;
    }
    len++;
    if (len == 10 && checksum_ok == 1) {
      *p10 = (float)pm10_serial / 10.0;
      *p25 = (float)pm25_serial / 10.0;
      len = 0; checksum_ok = 0; pm10_serial = 0.0; pm25_serial = 0.0; checksum_is = 0;
      error = 0;
    }
    yield();
  }
  return error;
}


//--------------------------------------------------------------------------------------
// 4  end
// -------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// 5  begin
//--------------------------------------------------------------------------------------
//  SetDeviceID
//--------------------------------------------------------------------------------------
//
static byte SetDeviceIDCmd[19] = {


  0xAA, // head
  0xB4, // command id
  0x05, // data byte 1
  0x00, // data byte 2
  0x00, // data byte 3
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12 (New device id byte 1)
  0x00, // data byte 13 (New device id byte 2)
  0xFF, // data byte 14 (exist device id byte 1) FF= all = read
  0xFF, // data byte 15 (exist device id byte 2) FF= all = read
  0x03, // checksum
  0xAB  // tail
};

//checksum = summe databytes 1-15 = 5+0+0+ff+ff=3




int SDS011_vers::SetDeviceID(int *DevID1n,  int *DevID2n,  int *DevID1r,  int *DevID2r  ) {
 

#ifdef DEBUG5

    Serial.println();
    Serial.println("_Begin SetDeviceID Main Function_");
    Serial.print("DevID1n "); Serial.print(*DevID1n, HEX); Serial.print("  DevID2n "); Serial.println(*DevID2n, HEX);
    Serial.print("DevID1r "); Serial.print(*DevID1r, HEX); Serial.print("  DevID2r "); Serial.println (*DevID2r, HEX);
  #endif

  SetDeviceIDCmd[13] = *DevID1n;
  SetDeviceIDCmd[14] = *DevID2n;
  SetDeviceIDCmd[15] = *DevID1r;
  SetDeviceIDCmd[16] = *DevID2r;


#ifdef DEBUG5
    Serial.print("DevID1n     ");  Serial.println(SetDeviceIDCmd[13], HEX);
    Serial.print("DevID2n     ");  Serial.println(SetDeviceIDCmd[14], HEX);
  #endif


  int checksum_is = 0;
  checksum_is = SetDeviceIDCmd[2] +  SetDeviceIDCmd[13] + SetDeviceIDCmd[14] + SetDeviceIDCmd[15] + SetDeviceIDCmd[16];
  SetDeviceIDCmd[17] = checksum_is % 256;

#ifdef DEBUG5
    Serial.print("checksum new ");  Serial.println(SetDeviceIDCmd[17]);
    Serial.print("Checksum_is % 256:  "); Serial.println(checksum_is % 256);
  #endif


  // write buffer
  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(SetDeviceIDCmd[i]);
  }
  sds_data->flush();  // write buffer to sensor

#ifdef DEBUG5
    Serial.println("buffer written");
  #endif

  //------------------------------------


  // read answer
  int antwort[9];
  byte buffer;
  int value;
  //  int len = 0;
  checksum_is = 0;
  int checksum_ok = 0;
  int error = 1;
  bool DataOK = false;
  int z = 0;

  //-------------------------

  //  Serial.println("warte auf Data Available 2");
  while (!sds_data->available() or z < 9 ) {
    delay(150); // Warten bis Daten da sind

    Serial.print(".");                                               // Problem endlos weil sich sensor ncach cmd abschaltet ???

    z++; if (z >= 9) {
      break;
    };
  }
  //Serial.println();

  if (  (sds_data->available() > 0) && (sds_data->available() >= 10 ) )
  {

    //Serial.println(" vordataok ");
    while (DataOK == false) {

      // Serial.println(" indataok ");
      buffer = sds_data->read();
      value = int(buffer);

#ifdef DEBUG5
        Serial.print("Value:  "); Serial.println(value, HEX);
      #endif
      if (value = 0xAA )
      {
        for (uint8_t z = 1; z < 10; z++) {
          buffer = sds_data->read();
          antwort[z] = int(buffer);

         #ifdef DEBUG5
            Serial.print(" 0x");          Serial.println(antwort[z], HEX);
          #endif
        }
      }

      if (antwort[1] == 0xC5 && antwort[2] == 5 && antwort[9] == 0xAB)
      {
        checksum_is = antwort[2] + antwort[3] + antwort[4] + antwort[5] + antwort[6] + antwort[7];

        #ifdef DEBUG5
          Serial.print(" Ver2 all hex antwort checksum_is checksum_is % 256 ");
          Serial.print(antwort[8], HEX); Serial.print(" = "); Serial.print(checksum_is, HEX); Serial.print(" = "); Serial.println(checksum_is % 256, HEX);
        #endif

        if (antwort[8] == checksum_is % 256)
        {

          *DevID1r = antwort[6];
          *DevID2r = antwort[7];
          DataOK = true;
          error = 0;
        } else {
          for (uint8_t z = 1; z < 10; z++) {
            antwort[z] = 0;
          }
        }
      }
    }  // end while dataok
  }


  //--------------------------
  yield();

#ifdef DEBUG5
    Serial.println(" ");
    Serial.print("DevID1n "); Serial.print(*DevID1n, HEX); Serial.print("  DevID2n "); Serial.print(*DevID2n, HEX);
    Serial.print("DevID1r "); Serial.print(*DevID1r, HEX); Serial.print("DevID2r "); Serial.println (*DevID2r, HEX);
    Serial.println("_End of SetDeviceID Function_");

#endif

  return error;
}  // END SDS011_vers::SetDeviceID
//-----------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// 5  end
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// 6  begin
// -------------------------------------------------------------------------------------
// SDS011_vers:SetSleepAndWork
// --------------------------------------------------------

static byte SLEEPCMD[19] = {
  0xAA, // head
  0xB4, // command id
  0x06, // data byte 1
  0x01, // data byte 2 ( 0 query mode) ( 1 set mode)
  0x00, // data byte 3 ( 0 sleep)       (1 work)
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1) FF= all
  0xFF, // data byte 15 (device id byte 2) FF= all
  0x05, // checksum
  0xAB  // tail
};

//checksum = summe databytes 1-15 = 6+1+0+ff+ff= 5    //



// --------------------------------------------------------
// SDS011_vers:SetSleepAndWork
// --------------------------------------------------------

int SDS011_vers::SetSleepAndWork(int *querysetc, int *sleepworkc, int *DevID1c, int *DevID2c, int *querysetr, int *sleepworkr, int *DevID1r, int *DevID2r) {

#ifdef DEBUG6
    Serial.println(" ");

    Serial.println("Begin SetSleepAndWork Main Function");
    Serial.print("querysetc  "); Serial.println(*querysetc);
    Serial.print("sleepworkc "); Serial.println(*sleepworkc);
    Serial.print("DevID1c    "); Serial.println(*DevID1c, HEX);
    Serial.print("DevID2c    "); Serial.println(*DevID2c, HEX);
  #endif

  SLEEPCMD[3]  = *querysetc;
  SLEEPCMD[4]  = *sleepworkc;
  SLEEPCMD[15] = *DevID1c;
  SLEEPCMD[16] = *DevID2c;


#ifdef DEBUG6
    Serial.print("queryset  [] ");  Serial.println(SLEEPCMD[3]);
    Serial.print("sleepwork [] ");  Serial.println(SLEEPCMD[4]);
  #endif


  int checksum_is = 0;
  checksum_is = SLEEPCMD[2] + SLEEPCMD[3] + SLEEPCMD[4] + SLEEPCMD[15] + SLEEPCMD[16];
  SLEEPCMD[17] = checksum_is % 256;



#ifdef DEBUG6
    Serial.print("checksum new        ");  Serial.println(SLEEPCMD[17]);
    Serial.print("Checksum_is % 256:  "); Serial.println(checksum_is % 256);
  #endif


  // write buffer
  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(SLEEPCMD[i]);
  }
  sds_data->flush();  // write buffer to sensor



#ifdef DEBUG6
    Serial.println("buffer written");
  #endif

  //------------------------------------



  // read answer
  int antwort[9];
  byte buffer;
  int value;
  checksum_is = 0;
  int checksum_ok = 0;
  int error = 1;
  bool DataOK = false;
  int z = 0;

  //-------------------------

  //  Serial.println("warte auf Data Available 2");
  while (!sds_data->available() or z < 9 ) {
    delay(150); // Warten bis Daten da sind
    // Serial.print(".");                                               // Problem endlos weil sich sensor ncach cmd abschaltet ???
    z++; if (z >= 9) {
      break;
    };
  }
  Serial.println();



  if (  (sds_data->available() > 0) && (sds_data->available() >= 10 )) // && (SLEEPCMD[4] == 0))
  {


    while (DataOK == false) {

      buffer = sds_data->read();
      value = int(buffer);

      //Serial.print("Value:  "); Serial.println(value, HEX);

      if (value = 0xAA )
      {
        for (uint8_t z = 1; z < 10; z++) {
          buffer = sds_data->read();
          antwort[z] = int(buffer);

          #ifdef DEBUG6
            Serial.print(" 0x");          Serial.println(antwort[z], HEX);
          #endif
        }
      }

      if (antwort[1] == 0xC5 && antwort[2] == 6 && antwort[9] == 0xAB)
      {
        checksum_is = antwort[2] + antwort[3] + antwort[4] + antwort[5] + antwort[6] + antwort[7];


       #ifdef DEBUG6
          Serial.print(" antwort = checksum_is = checksum_is % 256 ");
          Serial.print(antwort[8], HEX); Serial.print(" = "); Serial.print(checksum_is, HEX); Serial.print(" = "); Serial.println(checksum_is % 256, HEX);
        #endif

        if (antwort[8] == checksum_is % 256)
        {
          *querysetr  = antwort[3];
          *sleepworkr = antwort[4];

          *DevID1r = antwort[6];
          *DevID2r = antwort[7];
          DataOK = true;
          error = 0;
        } else {
          //      for (uint8_t z = 1; z < 10; z++) {
          //      antwort[z] = 0;
          //  }
        }
      }
    }  // end while dataok

   #ifdef DEBUG6
      Serial.println(antwort[3]);
      Serial.println(antwort[4]);
      Serial.println(antwort[6]);
      Serial.println(antwort[7]);
      Serial.print("querysetr  "); Serial.println(*querysetr);
      Serial.print("sleepworkr "); Serial.println(*sleepworkr);
      Serial.print("DevID1     "); Serial.println(*DevID1r, HEX);
      Serial.print("DevID2     "); Serial.println(*DevID2r, HEX);
      Serial.println("End of SetSleepAndWork Function");

    #endif


    //--------------------------
    yield();



  }
  return error;
}  // END SDS011_vers::SetSleepAndWork

//---------------------------------------------------------



// --------------------------------------------------------
// SDS011_vers:SetSleep()
// --------------------------------------------------------
int SDS011_vers::SetSleep() {
  int queryset = 1;
  int sleepwork = 0;
  int DevID1c = 0xFF;
  int DevID2c = 0xFF;
  int querysetr, sleepworkr, DevID1r, DevID2r;
  int _error = SDS011_vers::SetSleepAndWork(&queryset, &sleepwork, &DevID1c, &DevID2c, &querysetr, &sleepworkr, &DevID1r, &DevID2r);
  return _error;
}




// --------------------------------------------------------
// SDS011_vers:SetWork()
// --------------------------------------------------------
int SDS011_vers::SetWork() {
  sds_data->write(0x01);
  sds_data->flush();  //Puffer schreiben

  int queryset = 1;
  int sleepwork = 1;
  int DevID1c = 0xFF;
  int DevID2c = 0xFF;
  int querysetr, sleepworkr, DevID1r, DevID2r;
  int _error = SDS011_vers::SetSleepAndWork(&queryset, &sleepwork, &DevID1c, &DevID2c, &querysetr, &sleepworkr, &DevID1r, &DevID2r);
  return _error;
}


//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
// void  SDS011_vers:wakeup
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  wakeup
//--------------------------------------------------------------------------------
//
void SDS011_vers::wakeup() {
  sds_data->write(0x01);
  sds_data->flush();

  SLEEPCMD[4] = 1;


  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(SLEEPCMD[i]);
  }
  sds_data->flush();


  while (sds_data->available() > 0) {
    sds_data->read();
  }

}


//--------------------------------------------------------------------------------
//  void sleep()
//--------------------------------------------------------------------------------
//
void SDS011_vers::sleep() {
  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(SLEEPCMD[i]);
  }

  sds_data->flush();


    while (sds_data->available() > 0) {
      sds_data->read();
    }
  
}


//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// 6  end
// -------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// 7
//--------------------------------------------------------------------------------
//  check firmware version         FERTIG
//--------------------------------------------------------------------------------
//
static byte check_FirmwareVersionCmd[19] = {


  0xAA, // head
  0xB4, // command id
  0x07, // data byte 1
  0x00, // data byte 2
  0x00, // data byte 3
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1) FF= all
  0xFF, // data byte 15 (device id byte 2) FF= all
  0x05, // checksum
  0xAB  // tail
};

//checksum = summe databytes 1-15 = 7+0+0+ff+ff= 5


//--------------------------------------------------

int SDS011_vers::CheckFirmwareVersion(int *Y_ear, int *M_onth, int *D_ay,  int *D_evB1,  int *D_evB2)
{
  


  // Befehl sammeln und schreiben
  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(check_FirmwareVersionCmd[i]);
    //  Serial.print("  0x"); Serial.print(check_FirmwareVersionCmd[i], HEX);
  }
  sds_data->flush(); //Puffer schreiben


  //------------------------

  // Lesen
  int antwort[9];
  byte buffer;
  int value;
  int checksum_is = 0;
  int error = 3;
  int z = 0;

  //-------------------------

  //  Serial.println("warte auf Data Available 2");
  while (!sds_data->available() or z < 9 ) {
    delay(150); // Warten bis Daten da sind
    Serial.print(".");                                               // Problem endlos weil sich sensor ncach cmd abschaltet ???
    z++; if (z >= 9) {
      break;
    };
  }


  if (  (sds_data->available() > 0) && (sds_data->available() >= 10 ) )
  {
    //  Serial.println(" Data Available 2 ");
    buffer = sds_data->read();
    value = int(buffer);

    //   Serial.print("Value:  "); Serial.println(value, HEX);

    if (value = 0xAA )
    {
      for (uint8_t z = 1; z < 10; z++) {
        buffer = sds_data->read();
        antwort[z] = int(buffer);

       #ifdef DEBUG7
          Serial.print(" 0x");          Serial.println(antwort[z], HEX);
        #endif
      }
    }
  }
  // Serial.println("_");

  if (antwort[1] == 0xC5 && antwort[2] == 7 && antwort[9] == 0xAB)
  {
    checksum_is = antwort[2] + antwort[3] + antwort[4] + antwort[5] + antwort[6] + antwort[7];

    #ifdef DEBUG7
      Serial.print(" Ver2 all hex antwort checksum_is checksum_is % 256 ");    Serial.print(antwort[8], HEX); Serial.print(" = "); Serial.print(checksum_is, HEX); Serial.print(" = "); Serial.println(checksum_is % 256, HEX);
    #endif

    if (antwort[8] == checksum_is % 256)
    {
      *Y_ear  = antwort[3];       // Year
      *M_onth = antwort[4];       // Month
      *D_ay   = antwort[5];       // Day
      *D_evB1 = antwort[6];       // Device Byte 1
      *D_evB2 = antwort[7];       // Device Byte 2
      error = 0;
    } else {
      error = 7;
    }
  }

  yield();
  return error;
}



//--------------------------------------------------------------------------------------







//--------------------------------------------------------------------------------------
// 8
//--------------------------------------------------------------------------------------
//  ( effective after power off)              ( Continuous mode factory default )
//
// int SetWorkingPeriod(queryset,contperiod,DevID1c,DevID2c,querysetr,contperiodr,DevID1r,DevID2r)
//--------------------------------------------------------------------------------------

static byte SetWorkingPeriod_SDS_cmd[19] = {


  0xAA, // head
  0xB4, // command id
  0x08, // data byte 1
  0x01, // data byte 2 ( 0 query mode)  (1 set mode)
  0x00, // data byte 3 ( 0 cont  mode)  (1 - 30 【work 30 seconds and sleep n*60 - 30 seconds】)
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1) FF= all
  0xFF, // data byte 15 (device id byte 2) FF= all
  0x07, // checksum
  0xAB  // tail
};

//checksum = summe databytes 1-15 = 8+1+0+ff+ff= 7

//----------------------------------------------------



//----------------------------------------------------
int SDS011_vers::SetWorkingPeriod(int *queryset, int *contperiod, int *DevID1c, int *DevID2c, int *querysetr, int *contperiodr, int *DevID1r, int *DevID2r)
{

#ifdef DEBUG8

    Serial.println(" ");

    Serial.println("_Begin SetWorkingPeriod Main Function_");
    Serial.print("queryset "); Serial.print(*queryset, HEX); Serial.print("contperiod "); Serial.println(*contperiod, HEX);
    Serial.print("DevID1c "); Serial.print(*DevID1c, HEX); Serial.print("DevID2c "); Serial.println (*DevID2c, HEX);
  #endif

  SetWorkingPeriod_SDS_cmd[3] = *queryset;
  SetWorkingPeriod_SDS_cmd[4] = *contperiod;
  SetWorkingPeriod_SDS_cmd[15] = *DevID1c;
  SetWorkingPeriod_SDS_cmd[16] = *DevID2c;

 #ifdef DEBUG8
    Serial.print("queryset     ");  Serial.println(SetWorkingPeriod_SDS_cmd[3]);
    Serial.print("contperiod   ");  Serial.println(SetWorkingPeriod_SDS_cmd[4]);
  #endif


  int checksum_is = 0;
  checksum_is = SetWorkingPeriod_SDS_cmd[2] + SetWorkingPeriod_SDS_cmd[3] + SetWorkingPeriod_SDS_cmd[4] + SetWorkingPeriod_SDS_cmd[15] + SetWorkingPeriod_SDS_cmd[16];
  SetWorkingPeriod_SDS_cmd[17] = checksum_is % 256;

 #ifdef DEBUG8
    Serial.print("checksum new ");  Serial.println(SetWorkingPeriod_SDS_cmd[17]);
    //Serial.print("Checksum_is % 256:  "); Serial.println(checksum_is % 256);
  #endif


  // write buffer
  for (uint8_t i = 0; i < 19; i++) {
    sds_data->write(SetWorkingPeriod_SDS_cmd[i]);
  }
  sds_data->flush();  // write buffer to sensor


  #ifdef DEBUG8
    Serial.println("buffer written");
    Serial.println("__________________________________________________________________");
  #endif
  //------------------------------------


  // read answer
  int antwort[9];
  byte buffer;
  int value;
  //  int len = 0;
  checksum_is = 0;
  int checksum_ok = 0;
  int error = 1;
  bool DataOK = false;
  int z = 0;

  //-------------------------

  //  Serial.println("warte auf Data Available 2");
  while (!sds_data->available()  or z > 9) {
    delay(150); // Warten bis Daten da sind
    Serial.print(".");
    z++; if (z >= 9) {
      break;
    };
  }
  Serial.println();

  if (  (sds_data->available() > 0) && (sds_data->available() >= 10 ) )
  {

    //Serial.println(" vordataok ");
    while (DataOK == false) {

      // Serial.println(" indataok ");
      buffer = sds_data->read();
      value = int(buffer);

      // Serial.print("Value:  "); Serial.println(value, HEX);

      if (value = 0xAA )
      {
        for (uint8_t z = 1; z < 10; z++) {
          buffer = sds_data->read();
          antwort[z] = int(buffer);
         #ifdef DEBUG8
            Serial.print(" 0x");          Serial.println(antwort[z], HEX);
          #endif
        }
      }

      //Serial.println("__________________________________________________________________");

      if (antwort[1] == 0xC5 && antwort[2] == 8 && antwort[9] == 0xAB)
      {
        checksum_is = antwort[2] + antwort[3] + antwort[4] + antwort[5] + antwort[6] + antwort[7];

        //Serial.print(" Ver2 all hex antwort checksum_is checksum_is % 256 ");
        //Serial.print(antwort[8], HEX); Serial.print(" = ");Serial.print(checksum_is, HEX);Serial.print(" = ");Serial.println(checksum_is % 256, HEX);

        if (antwort[8] == checksum_is % 256)
        {
          *querysetr  = antwort[3];
          *contperiodr = antwort[4];

          *DevID1r = antwort[6];
          *DevID2r = antwort[7];
          DataOK = true;
          error = 0;
        } else {
          for (uint8_t z = 1; z < 10; z++) {
            antwort[z] = 0;
          }
        }
      }
    }  // end while dataok
  }


  //--------------------------
  yield();

#ifdef DEBUG8
    Serial.print("_querysetr_"); Serial.print(*querysetr, HEX); Serial.print("_contperiodr_"); Serial.print(*contperiodr, HEX);
    Serial.print("_DevID1_"); Serial.print(*DevID1r, HEX); Serial.print("_DevID2_"); Serial.println (*DevID2r, HEX);
    Serial.println("_End of SetWorkingPeriod Function_");
  #endif
  return error;
}  // END SDS011_vers::SetWorkingPeriod


//--------------------------------------------------------------------------------







// --------------------------------------------------------
// int SDS011_vers::SetContinuousMode()
// --------------------------------------------------------
int SDS011_vers::SetContinuousMode() {


  int queryset = 1;
  int contperiod = 0;
  int DevID1c = 0xFF;
  int DevID2c = 0xFF;
  int querysetr, contperiodr, DevID1r, DevID2r;

#ifdef DEBUG81
    Serial.println("__________________________________________________________________");
    Serial.println("_Begin of SetContinuousMode_");
    Serial.print("_queryset_"); Serial.print(queryset, HEX); Serial.print("_contperiod_"); Serial.println(contperiod, HEX);
    Serial.print("_DevID1c_"); Serial.print(DevID1c, HEX); Serial.print("_DevID2c_"); Serial.println (DevID2c, HEX);
  #endif

  int error = SDS011_vers::SetWorkingPeriod(&queryset, &contperiod, &DevID1c, &DevID2c, &querysetr, &contperiodr, &DevID1r, &DevID2r);

#ifdef DEBUG81
    Serial.print("_querysetr_"); Serial.print(querysetr, HEX); Serial.print("_contperiodr_"); Serial.println(contperiodr, HEX);
    Serial.print("_DevID1r_"); Serial.print(DevID1r, HEX); Serial.print("_DevID2r_"); Serial.println (DevID2r, HEX);
    Serial.println("_End of SetContinuousMode_");
    Serial.println("__________________________________________________________________");
  #endif
  return error;
}

//--------------------------------------------------------------------------------

// --------------------------------------------------------
// int SDS011_vers::SetWorkingPeriod3Min()
// --------------------------------------------------------
int SDS011_vers::SetWorkingPeriod3Min() {

  int queryset = 1;
  int contperiod = 5;
  int DevID1c = 0xFF;
  int DevID2c = 0xFF;
  int querysetr, contperiodr, DevID1r, DevID2r;

#ifdef DEBUG82
    Serial.println("__________________________________________________________________");
    Serial.println("_Begin of SetWorkingPeriod5Min_");
    Serial.print("_queryset_"); Serial.print(queryset, HEX); Serial.print("_contperiod_"); Serial.print(contperiod, HEX);
    Serial.print("_DevID1c_"); Serial.print(DevID1c, HEX); Serial.print("_DevID2c_"); Serial.println (DevID2c, HEX);
  #endif

  int _error = SDS011_vers::SetWorkingPeriod(&queryset, &contperiod, &DevID1c, &DevID2c, &querysetr, &contperiodr, &DevID1r, &DevID2r);

#ifdef DEBUG82
    Serial.print("_querysetr_"); Serial.print(querysetr, HEX); Serial.print("_contperiodr_"); Serial.print(contperiodr, HEX);
    Serial.print("_DevID1r_"); Serial.print(DevID1r, HEX); Serial.print("_DevID2r_"); Serial.println (DevID2r, HEX);
    Serial.println("_End of SetWorkingPeriod5Min_");
    Serial.println("__________________________________________________________________");
  #endif
  return _error;
}

//--------------------------------------------------------------------------------

//---EOF----------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------




