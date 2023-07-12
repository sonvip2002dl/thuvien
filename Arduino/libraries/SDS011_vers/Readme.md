Implementation of NovaFitness Protocol 1.3 for SDS011 Particle Matter Sensor

for ESP8266 with arduino IDE


based on the idea of https://github.com/ricki-z/SDS011  
and 
https://github.com/opendata-stuttgart/sensors-software/tree/master/airrohr-firmware
and
https://github.com/opendata-stuttgart/sensors-software/blob/master/airrohr-firmware/Readme.md

expanded to all documented functions.



Most libraries are based on the lib of ricki_z and work fine 
as they are reading works on factory defaults (data reporting mode active) (Set working period continuous mode)
they read what is available on UART.

Here I try to read in "data reporting query mode" 
which I could not get values with existing libraries and hope to have better control of running and pause.

NovaFitness Protocol Notes:

Report active mode: 
"Sensor automatically reports a measurement data in a work period."

Report query mode: 
"Sensor received query data command to report a measurement data."

Set Sleep Work: 
"The setting is not effective after power off【stay work state after power on】 
Any command from UART will wake it up when the sensor is sleeping. 
The data is stable when the sensor works after 30 seconds; 
The fan and laser stop working in sleeping mode."

Set Working Period: 
"The setting is still effective after power off【factory default is continuous measurement】 
The sensor works periodically and reports the latest data."

Query data command: 
"Sensor received query data command to report a measurement data, 
recommended query interval of not less than 3 seconds"


As far as I tried, report active mode with set working period 3 min work fine with 30 sec read,fan on, and 2min 30 sec sleep, fan off.

data reporting query mode with set woking period: continuous mode, 0 min, 
work fine in the library test sketch: set work, read command, fan on for 3 seconds, set sleep, fan off, delay(20000)

Alas, inside a sketch (Mobisens), sometimes it doesn't stop the fan.


It is to compare if and how data quality changes when reading 3 s (query reporting mode) or 30 s (active reporting mode)


.. still in working progress ...


Reference: https://cdn.sparkfun.com/assets/parts/1/2/2/7/5/Laser_Dust_Sensor_Control_Protocol_V1.3.pdf


Similar and maybe more elegant implementations are recently found on 


https://github.com/kadamski/arduino_sds011

https://github.com/Zanop/SDS011/tree/more-sensor-modes

https://github.com/gitpeut/SDS021

https://github.com/lewapek/sds-dust-sensors-arduino-library








