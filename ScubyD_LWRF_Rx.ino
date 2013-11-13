// Last Updated by David Walker, 13 Nov 2013
//
// This Rx test file is based on the mid-Nov 2013 version of
// RobertTidey's LWRF library.  I have made very few changes
// to this code, but I've included it for completeness.
//
// Note that I renamed his libraries (which includes updating
// the name of the .h file in the .cpp file) in order to
// maintain version control between the multiple LWRF-related
// libraries I have in my Arduino IDE.

#include <Tidey2_LwRx.h>

void setup() {
  // set up with rx into pin 2
  lwrx_setup(2);
  Serial.begin(9600);
  Serial.println("Set up completed and stats enabled");
}

void loop() {
  byte msg[10];
  byte len = 10;
  int inchar;

  if (lwrx_message()) {
    lwrx_getmessage(msg,&len);
    printMsg(msg, len);
  }
  if(Serial.available()) {
    inchar = Serial.read();
    if (inchar == 48) { //0 print stats
      printStats();
    } else if (inchar == 49) { //1 clear stats and reenable
      lwrx_setstatsenable(false);
      lwrx_setstatsenable(true);
      Serial.println("Stats reset and enabled");
    } else if (inchar == 50) { //2 turn off stats
      lwrx_setstatsenable(false);
      Serial.println("Stats disabled");
    }
  }
}

void printMsg(byte *msg, byte len) {
  Serial.print(millis());
  Serial.print(" ");
  for(int i=0;i<len;i++) {
    Serial.print(msg[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void printStats() {
  unsigned int stats[rx_stat_count];
  
  if (lwrx_getstats(stats)) {
    Serial.print("Stats, ");
    for (byte i=0; i<rx_stat_count; i++) {
      // Ave values 0,3,6 are 16 x
      if (i % 3 == 0) {
        Serial.print(stats[i] >> 4);
      } else {
        Serial.print(stats[i]);
      }
      Serial.print(",");
    }
    Serial.println();
  } else {
    Serial.println("No stats available");
  }
}
