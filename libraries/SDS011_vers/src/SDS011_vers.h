//----------------------------------------------------------------
//----------------------------------------------------------------
// SDS011_vers.h dust sensor PM2.5 and PM10
// ---------------------------------------------------------------
//
// idea based on SDS011 By R. Zschiegner (rz@madavi.de)
// and extended based on 
// Documentation:
// The iNovaFitness Laser Dust Sensor Control Protocol V1.4 SDS011
//
//----------------------------------------------------------------
//----------------------------------------------------------------
#ifndef SDS011_vers_h
#define SDS011_vers_h
//----------------------------------------------------------------
// #define DEBUG  // DEBUG2 4 5 6 7 8 81 81
//----------------------------------------------------------------

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

//----------------------------------------------------------------

#include <SoftwareSerial.h>

//----------------------------------------------------------------

class SDS011_vers {
//----------------------------------------------------------------
  public:

    SDS011_vers(void);

    void begin(uint8_t pin_rx, uint8_t pin_tx);

    void begin(SoftwareSerial* serial);

	// void begin(HardwareSerial* serial);

//----------------------------------------------------------------


// 1


// 2
    int SetDataReportingMode(int *queryset,int *activequery,int *DevID1c,int *DevID2c,int *querysetr,int *activequeryr,int *DevID1r,int *DevID2r);

    int SetQueryReportingMode();

    int SetActiveReportingMode();

// 3



// 4
    int QueryDataCommand( int *DevID1c,int *DevID2c,float *p25, float *p10,int *DevID1r,int *DevID2r);

    int read_q(float *p25, float *p10);

    int read_a(float *p25, float *p10);

// 5
    int SetDeviceID(int *DevID1n,  int *DevID2n,  int *DevID1r,  int *DevID2r  );


// 6
    int SetSleepAndWork(int *queryset,int *sleepwork,int *DevID1c,int *DevID2c,int *querysetr,int *sleepworkr,int *DevID1r,int *DevID2r  );

    int SetSleep();

    int SetWork();

    void wakeup();

    void sleep();


// 7   fertig

    int  CheckFirmwareVersion(int  *Y_ear, int *M_onth, int *D_ay,  int *D_evB1,  int *D_evB2  );

// 8
    int SetWorkingPeriod( int *queryset, int *contperiod, int *DevID1c, int *DevID2c, int *querysetr, int *contperiodr, int *DevID1r, int *DevID2r ); 

    int SetContinuousMode();

    int SetWorkingPeriod3Min();

//----------------------------------------------------------------
  private:
    uint8_t _pin_rx, _pin_tx;
    Stream *sds_data;



//----------------------------------------------------------------
};

#endif
//----------------------------------------------------------------
//----------------------------------------------------------------
