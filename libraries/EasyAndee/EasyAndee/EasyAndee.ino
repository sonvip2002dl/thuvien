#include <SPI.h>
#include <EasyAndee.h>

void setup() {
  // put your setup code here, to run once:
  EasyAndeeBegin();
  setName("EZ Andee");
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  EasyAndeePoll();
}
