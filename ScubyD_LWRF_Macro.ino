// Last Updated by David Walker, 23 Nov 2013
//
// This xxxxxxxxx is based on the late-Nov 2013 version of
// RobertTidey's LWRF library.
//
// Nano = Board: Nano w/ Mega328  +  Programmer: AVRISP mkII
// Mini = Pro Mini 5V 16Mhz w/ Mega328

#include <Tidey3_LwTx.h>
#include <Tidey3_LwRx.h>
#include <EEPROM.h>



char rxReturnHome1[11] = "0001GGGGG7"; // Mood On (0/1)
char rxReturnHome2[11] = "xx0xGGGGG7"; // Black Remote A1
char txReturnHome[1][11] = {
  "82F2GGGGG1", //Rm2 All On (all networks on)
};
long txReturnHomeWaits[0];
int txNumReturnHome = 1;

char rxLeaveHouse1[11] = "4000GGGGG7"; // Mood Off (0/1)
char rxLeaveHouse2[11] = "xx2xGGGGG7"; // Black Remote A3
char txLeaveHouse[14][11] = {
  "0041GGGGG0", //Rm1D5 On (front porch light)
  "0051GGGGG0", //Rm1D6 On (front garage light)
  "C000GGGGG6", //Rm7 AllOff (upstairs lights)
  "4000GGGGG3", //Rm4D1 Off (TV unit off)
  "4010GGGGG3", //Rm4D2 Off (Airplay off)
  "4040GGGGG3", //Rm4D5 Off (iPhone charger off)
  "C000GGGGG7", //Rm8 AllOff (unused sockets)
  "C000GGGGG2", //Rm3 AllOff (living lights)
  "4020GGGGG1", //Rm2D3 Off (extra network off)
  "C000GGGGG5", //Rm6 AllOff (misc)
  "WAIT      ",
  "C000GGGGG4", //Rm5 AllOff (downstairs lights)
  "WAIT      ",
  "C000GGGGG0", //Rm1 AllOff (outside)
};
long txLeaveHouseWaits[2] = {10000, 90000};
int txNumLeaveHouse = 14;

char rxBedtime[11] = "84F2GGGGG7";  // Mood 3
char txBedtime[10][11] = {
  "82F2GGGGG6", //Rm7 Mood1 (set upstairs lights)
  "0041GGGGG3", //Rm4D5 On (iPhone charger)
  "4000GGGGG3", //Rm4D1 Off (TV unit off)
  "4010GGGGG3", //Rm4D2 Off (Airplay off)
  "C000GGGGG0", //Rm1 AllOff (outside)
  "C000GGGGG7", //Rm8 AllOff (unused sockets)
  "C000GGGGG5", //Rm6 AllOff (misc)
  "C000GGGGG2", //Rm3 AllOff (living lights)
  "WAIT      ",
  "C000GGGGG4", //Rm5 AllOff (downstairs lights)
};
long txBedtimeWaits[1] = {10000};
int txNumBedtime = 10;

char rxAllOff1[11] = "C0F0GGGGG7"; // Mood All Off
char rxAllOff2[11] = "xx3xGGGGG7"; // Black Remote A4
char txAllOff[][11] = {
  "C000GGGGG0", //Rm1 AllOff (outside)
  "C000GGGGG3", //Rm4 AllOff (entertainment)
  "C000GGGGG5", //Rm6 AllOff (misc)
  "C000GGGGG7", //Rm8 AllOff (unused)
  "C000GGGGG6", //Rm7 AllOff (lightsup)
  "C000GGGGG2", //Rm3 AllOff (livinglights)
  "C000GGGGG4", //Rm5 AllOff (lightsdown)
  "C000GGGGG1", //Rm2 AllOff (networks)
};
long txAllOffWaits[0];
int txNumAllOff = 8;

// Setup variables for message queue
//  NOTE: Queue is not emptied after sending, but earliestExec and numInQueue
//  determine whether there is anything to be sent
char msgStrQueue[25][11];  // Queue of ASCII commands waiting to be sent
long earliestSend[25];     // Aligns to msgStrQueue, gives earliest time to send
boolean waitingToSend[25]; // Aligns to msgStrQueue, true if msg is pending
int numInQueue = 0;        // Lists num pending messages in queue
long nextTxAllowed = 0;    // Millis when next Tx allowed
long minGap = 1000;        // Min gap between Tx messages
char waitStr[5] = "WAIT";




void setup() {
  // set up with rx into pin 2
  lwrx_setup(2);
  // setup tx on pin 3
  lwtx_setup(3, 10, 0, 140);
  Serial.begin(9600);
  Serial.println("Ready!");
}

void loop() {
  // Define variables
  byte rxmsg[10];
  byte txmsg[10];
  byte len = 10;
  char rxmsgstr[10];
  char txmsgstr[10];

  // Receive messages and do immediate response
  if (lwrx_message()) {
    // Get the received message
    lwrx_getmessage(rxmsg, len);
    // Convert the message into a char string
    msgToStr(rxmsgstr, rxmsg);
    Serial.print("Received: ");
    Serial.println(rxmsgstr);
    // Check for triggers
    if ((isMatch(rxmsgstr, rxLeaveHouse1, 10)) || (isMatch(rxmsgstr, rxLeaveHouse2, 10)))
    {
      Serial.println("Received: Leaving Home");
      addToQueue(txLeaveHouse, txLeaveHouseWaits, txNumLeaveHouse);
    }
    else if ((isMatch(rxmsgstr, rxReturnHome1, 10)) || (isMatch(rxmsgstr, rxReturnHome2, 10)))
    {
      Serial.println("Received: Returning Home");
      addToQueue(txReturnHome, txReturnHomeWaits, txNumReturnHome);
    }
    else if (isMatch(rxmsgstr, rxBedtime, 10))
    {
      Serial.println("Received: Bed Time");
      addToQueue(txBedtime, txBedtimeWaits, txNumBedtime);
    }
    else if ((isMatch(rxmsgstr, rxAllOff1, 10)) || (isMatch(rxmsgstr, rxAllOff2, 10)))
    {
      Serial.println("Received: All Off");
      addToQueue(txAllOff, txAllOffWaits, txNumAllOff);
    }
  }
  
  // Do queued responses
  if (nextTxAllowed <= millis())
  {
    if (numInQueue > 0)
    {
      for (int i=0; i<25; i++)
      {
        if (waitingToSend[i]==true){
          if (earliestSend[i] <= millis())
          {
            strToMsg(txmsg, msgStrQueue[i]);
            lwtx_send(txmsg);
            Serial.print("Sent: ");
            Serial.println(msgStrQueue[i]);
            nextTxAllowed = millis() + minGap;
            numInQueue--;
            waitingToSend[i] = false;
            earliestSend[i] = 0;
            msgStrQueue[i][0] = ' ';
            break;
          }
        }
      }
    }
  }
  
  
  
}


// Work through the txList, adding msgs to the queue as needed,
//  including any delays requested
void addToQueue(char txList[][11], long txListWaits[], int listLen)
{
  // Note the current time as earliest
  long earliestTime = millis();
  // Keep track of whether any wait commands have been encountered
  int waitNum = 0;
  // Work through the txList in order
  for (int iList=0; iList<listLen; iList++)
  {
    Serial.print("  Queuing message: ");
    Serial.println(txList[iList]);
    // If the entry is "WAIT", then get the next entry from txListWaits
    if (isMatch(txList[iList], waitStr, 4))
    {
      earliestTime += txListWaits[waitNum];
      waitNum++;
    }
    // If isn't a wait command, then add it to the queue
    else
    {
      // Look through the queue to find the first empty gap
      for (int i=0; i<25; i++)
      {
        // Need an entry that isn't currently pending
        if (!waitingToSend[i])
        {
          // Add the txList entry to the queue
          for (int j=0; j<10; j++)
          {
            msgStrQueue[i][j] = txList[iList][j];
          }
          // Save the earliest send time
          earliestSend[i] = earliestTime;
          // Flag as waiting to send
          waitingToSend[i] = true;
          // Increment the number in queue
          numInQueue++;
          // Finished, so drop out of the for loop
          break;
        }
      }
    }
  }
}






// Compare two msg strings, return false if any
//  character doesn't match (only first [len])
//  'x' is a wildcard
boolean isMatch(char *msg1, char *msg2, int len)
{
  for (int i=0; i<len; i++)
  {
    if ((msg1[i] != msg2[i]) && (msg1[i] != 'x') && (msg2[i] != 'x'))
    {
      return false;
    }
  }
  return true;
}

// Converts byte msg to char string
void msgToStr(char msgstr[10], byte *msg)
{
  for(int i=0; i<10; i++)
  {
    msgstr[i] = getHexChar(msg[i]);
  }
}

// Converts char string to byte msg
void strToMsg(byte msg[10], char* msgstr)
{
  for (int i=0; i<10; i++)
  {
    msg[i] = getHexValue(msgstr[i]);
  }
}

// Convert ASCII hex text to 0-15 integer values
//  (for String to Message translation)
int getHexValue(char ascii)
{
  if (ascii > 0x39) ascii -= 7;
  return (ascii & 0xf); 
}

// Convert 0-15 integer values to ASCII hex text
//  (for Message to String translation)
char getHexChar(int hex)
{
  switch (hex)
  {
    case 0: return '0'; break;
    case 1: return '1'; break;
    case 2: return '2'; break;
    case 3: return '3'; break;
    case 4: return '4'; break;
    case 5: return '5'; break;
    case 6: return '6'; break;
    case 7: return '7'; break;
    case 8: return '8'; break;
    case 9: return '9'; break;
    case 10: return 'A'; break;
    case 11: return 'B'; break;
    case 12: return 'C'; break;
    case 13: return 'D'; break;
    case 14: return 'E'; break;
    case 15: return 'F'; break;  
  }
}

