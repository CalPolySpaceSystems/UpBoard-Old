#include <Arduino.h>
#include "avdweb_SAMDtimer.h"
#include "CircBuffer.h"
#include "gps.h"

#define SerialGPS Serial

/* Setup timer interrupt with period of 8/9600s */
CircBuffer the_ring;

void ISR_print(struct tc_module *const module_inst) {
   if(the_ring.available()) {
      SerialUSB.write(the_ring.read_char());
   }
}

SAMDtimer timer3_9600 = SAMDtimer(4, ISR_print, (8e6/9600));
uint8_t temp;

void setup() {
   SerialGPS.begin(9600);
   SerialUSB.begin(9600);
   setupGPSNMEAStrings(&SerialGPS, &SerialUSB);
   delay(10);
   flushGPS(&SerialGPS);
}

void loop() {
   if(SerialGPS.available()) {
     temp = (uint8_t)SerialGPS.read();
     the_ring.store_char(temp);
   }
}
