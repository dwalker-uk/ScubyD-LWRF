// Last Updated by David Walker, 13 Nov 2013
//
// This Tx test file is based on the mid-Nov 2013 version of
// RobertTidey's LWRF library.  I have changed his code by
// removing the pre-defined message and replacing it with
// my own code which allows the Arduino Serial Monitor to be
// used to enter codes directly.
// Codes entered in the Serial Monitor should be exactly 10
// characters long, with no spaces and containing only valid
// hex characters (i.e. 0-9 and A-F).  These are translated
// into their corresponding integer values (0-15) before being
// transmitted using Robert Tidey's LWRF library functions.
//
// Note that I renamed his libraries (which includes updating
// the name of the .h file in the .cpp file) in order to
// maintain version control between the multiple LWRF-related
// libraries I have in my Arduino IDE.

#include <Tidey2_LwTx.h>

byte msg[10];
char str[10];

void setup() {
  Serial.begin(9600);
  //Transmit on pin 3, 10 repeats,no invert, 140uSec tick)
  lwtx_setup(3, 10, 0, 140);
}

void loop() {
  // Only continue if transmission is ready / available
  if (lwtx_free()) {
    // Only respond to messages at least 10 characters long
    if (Serial.available() >= 10) {
      // Loop through each character, saving the original ascii to 'str'
      // and saving the int value of the ascii hex characters to 'msg'
      for(int i=0;i<10;i++) {
        str[i] = Serial.peek();
        msg[i] = getHexValue(Serial.read());
      }
      // Print the original ascii string to screen
      Serial.print("Sending: ");
      Serial.print(str);
      Serial.print(" -> ");
      // Print each converted integer value in turn
      for(int i=0;i<10;i++) {
        // Pad single digits with an extra space
        if(msg[i] < 10) Serial.print(" ");
        Serial.print(msg[i]);
        Serial.print(" ");
      }
      Serial.println();
      // Transmit the message
      lwtx_send(msg);
      // Clear any remaining characters from Serial (clear port)
      while (Serial.available()) Serial.read();
    }
  }
}

// Convert ASCII hex text entry in Serial Console to 0-15 integer values
int getHexValue(unsigned char ascii)
{
  if(ascii > 0x39) ascii -= 7;
  return (ascii & 0xf); 
}
