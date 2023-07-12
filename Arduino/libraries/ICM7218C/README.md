# ICM7218C
Library to interface with Maxim ICM7218C LED driver chips. Compatible with Arduino.  Modified from https://github.com/Andy4495/ICM7218/

/* Library to interface with Maxim ICM7218C LED driver chip.
   
   Original library by Andreas Taylor <Andy4495@outlook.com>

   Note that there are several chips in the 72xx family, including
   chips from Maxim.
   While these other chips provide similar capabilities, this library
   was specifically designed for the Maxim ICM7218C variant.
   This library will function properly with other A/B chips and may
   not function out of the box for the D varient..
   https://facebook.com/werelwolf

   Interface requires up to 10 digital out pins for full support of
   all functionality. However, it is possible to use as few as 7 output
   pins if no decimal point is needed and are only driving 2 segment(s).
   Library has been coded to require at minimum two 7-segment displays.
*/

/* Version History
   1.0.0    02/12/2018  A.T.   Original
   1.1.0    02/27/2018  A.T.   Added support for ascii to segment mapping
   1.2.0    08/07/2018  A.T.   Changed names of shutdown() and wakeup() to
                               displayShutdown() and displayWakeup()
                               Renamed segment_map to ICM7218_segment_map
   1.2.3    03/23/2022  W.W.   Changing library for Maxim ICM7218C chip.
*/
