#include <Arduino.h>
#include "avdweb_SAMDtimer.h"

void ISR_print(struct tc_module *const module_inst) {
   Serial.print("a\n");
}

/* Setup timer interrupt with period of 8/9600 */
SAMDtimer timer3_9600 = SAMDtimer(4, ISR_print, 834);

void setup {
   SerialUSB.begin(9600);
}

void loop {

}
