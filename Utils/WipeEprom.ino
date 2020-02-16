// Use this sketch to clear the dongle's Eprom in case you lose the pincode. The password will be lost - there is no easy way to recover it.

#include <M5StickC.h>
#include "EEPROM.h"
#define EEPROM_SIZE 64
void setup() {
  while (!EEPROM.begin(EEPROM_SIZE)) true;
  for (int i=0;i<EEPROM_SIZE;++i)
    EEPROM.writeChar(i,0);
  EEPROM.commit();
}

void loop() {
  // put your main code here, to run repeatedly:

}
