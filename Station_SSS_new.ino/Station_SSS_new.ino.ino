
#include <sss7.h>
#include <Arduino.h>
#include "ardusss7.h"
#include "Keypad.h"
#include "ss7content.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

//#define MODE_SENDING_INFORMATION 99



//Keypad Definitions
const byte ROWS = 4; //four row
const byte COLS = 4; //four columns
char keys[ROWS][COLS] =
{ {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//Define Row Pins
byte rowPins[ROWS] = {
  33, 31, 29, 27
};
//Define Col Pins
byte colPins[COLS] = {
  32, 30, 28, 26
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal lcd(38, 36, 52, 50, 48, 46 );

//Declaring Variables
int blinking;

uint8_t ID;
uint8_t Page = 0;
uint8_t Mode;

char key="X" ;
int nkey;
char key1= "Y" ;
int nkey1;
int dest = -1;

int checkkey;

char type = 'Z';

unsigned long TimePlusDelayNextPage;
unsigned long TimePlusDelaySending;
unsigned long TimePlusDelayRecieving;


unsigned long DelayInitDialog = 2000;
unsigned long DelayTimeWhenSending = 80000;
unsigned long DelayTimeWhenRecieving = 20000;

bool SomeBoolVariable;
bool SomeOtherBoolVariable;
bool SomeFurtherBoolVariable;

uint8_t msg[SSS7_PAYLOAD_SIZE];
uint8_t msg1[SSS7_PAYLOAD_SIZE];

void setup()
{
  //Turn On LCD
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);



  //   ID = EEPROM.read(1);
  //  pinMode(22, OUTPUT);
  //  digitalWrite(22, HIGH);
  ID = 2;

  //LCD Start
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.print("init");
  //SSS7 Start
  SSS7.init();

  //Print Message
  lcd.setCursor(12, 0);
  lcd.print("ID=");
  lcd.setCursor(15, 0);
  lcd.print(ID);
  lcd.setCursor(0, 0);

  lcd.print("HELLO!");

  delay(1000);
  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("FOMPWAT");



  delay(1000);


}


void loop()
{
  

  switch (Mode) {
    case 0:
      //reset
      TimePlusDelayNextPage = millis() + DelayInitDialog;
      Mode = 1;
      break;


    case 1:
      //Display Text stuff

      if (Page == 0) {
        lcd.setCursor(0, 0);
        lcd.print("HOW to SEND         ");
        lcd.setCursor(0, 1);
        lcd.print("PRESS:           ");
      }

      if (Page == 1) {
        lcd.setCursor(0, 0);
        lcd.print("A FOR DATAMATRIX    ");
        lcd.setCursor(0, 1);
        lcd.print("B FOR ADDRESS       ");
      }

      if (Page == 2) {
        lcd.setCursor(0, 0);
        lcd.print("FOR HELP VISIT        ");
        lcd.setCursor(0, 1);
        lcd.print("silkroad.x-ra.de    ");
      }


      type = keypad.getKey();

      if (type == 'A') {
        Mode = 6;
        Page = 0;
      }

      if (type == 'B') {
        Mode = 2;
        Page = 0;
      }

      if (millis() > TimePlusDelayNextPage) {
        Page++;
        lcd.clear();
        TimePlusDelayNextPage = millis() + DelayInitDialog;
      }

      if (Page == 3) {
        Page = 0;
      }
      break;

    case 2: //INSERT DESTINATION
      lcd.clear();
      lcd.print("DESTINATION:_##____     ");


      key = keypad.waitForKey();
    
      
        lcd.setCursor(13, 0);
        lcd.print(key);
          lcd.setCursor(15, 0);
    lcd.print("_");
        Mode = 3;

      break;


    case 3: //INSERT DESTINATION
      lcd.print("DESTINATION:_ #___      ");
      key1 = keypad.waitForKey();
      
        lcd.setCursor(14, 0);
        lcd.print(key1);
     
        Mode = 4;
        nkey = (int)key;
        nkey1 = (int)key1;
        //dest = (nkey - 48) * 10 + (nkey1 - 48);
           dest = (nkey - '0') * 10 + (nkey1 -'0');   
      //Serial.println(dest);
      break;

    case 4:
      lcd.setCursor(0, 1);
      lcd.print(" # TO CONFIRM         ");

      if (keypad.getKey() == '#') {
        lcd.clear();
        lcd.print((int)dest);
        Mode = 7;
      }
      break;

    case 6: //DATAMATRIX SEND DIALOG
      lcd.clear();

      lcd.print("CODE ON CAPSULE?     ");
      lcd.setCursor(0, 1);
      lcd.print(" # TO CONFIRM        ");

      if (keypad.getKey() == '#') {
        lcd.clear();
        Mode = 7;
        dest = -1;
      }
      break;

    case 7:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DO NOT INSERT       ");
      blinking = 0;
      while (blinking < 10) {
        blinking++;
        digitalWrite(22, LOW);
        delay(200);
        digitalWrite(22, HIGH);
        delay(200);
      }
      Mode = 99;
      break;

    //Error case
    case 42:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ERROR TIMEOUT         ");
      lcd.setCursor(0, 1);
      lcd.print("CALL SILK               ");
      while (blinking < 20) {
        blinking++;
        digitalWrite(13, LOW);
        delay(100);
        digitalWrite(13, HIGH);
        delay(100);
      }
      Mode = 0;
      break;


    case 99://///////////////////////////////////////////////////////////
      TimePlusDelaySending = millis() + DelayTimeWhenSending;

      SomeBoolVariable = false;

      if (dest == -1) {
        msg[0] = MSG_REQUEST_BARCODE;
        msg[1] = ID;
        msg[2] = ID_COORDINATOR;
        msg[3] = 0;

        Serial.println("Waiting to send");
        while (!SSS7.canSend());
        Serial.println("Sending ...");
        SSS7.send(msg);

        while (!SSS7.canSend());
        SomeBoolVariable = SSS7.sendFailed();
        Serial.println("Finished Sending");
        Serial.println(SomeBoolVariable);
        // break;

        if (SomeBoolVariable==true) {
          Serial.println("Send failed");
          Mode = 42;
        }


      } else {
        SomeBoolVariable = false;
        msg[0] = MSG_REQUEST_DIRECT;
        msg[1] = ID;
        msg[2] = ID_COORDINATOR;
        msg[3] = dest;

        Serial.println("Waiting to send");
        while (!SSS7.canSend());
        Serial.println("Sending ...");
        SSS7.send(msg);

        while (!SSS7.canSend());
        SomeBoolVariable = SSS7.sendFailed();
        Serial.println(SomeBoolVariable);
        Serial.println("Finished Sending");
        // break;


        if (SomeBoolVariable) {
          Serial.println("Send failed");
          Mode = 42;

        }


      }

      if (millis() > TimePlusDelaySending && SomeBoolVariable) {
        Mode = 42;
      } else if (millis() <= TimePlusDelaySending && SomeBoolVariable) {
        Mode = 99;
      } else if (SomeBoolVariable == false) {
        Mode = 100;
      }  ;

      break;
    case 100://////////////////////////////////////////////////////////////
    Serial.println("Zustand 100");
      TimePlusDelayRecieving = millis() + DelayTimeWhenRecieving;
      SomeOtherBoolVariable = true;
      delay(200);
      if (SSS7.hasReceived()) {
        
        uint8_t msg1[SSS7_PAYLOAD_SIZE];
        SSS7.getReceived(msg1);

        Serial.print("Got data:");
        Serial.println((char*)msg1);
        if ((msg1[0] == MSG_CONFIRM_REQUEST) && (msg1[2] == ID)) {
          SomeOtherBoolVariable = false;
          Serial.println(msg1[0]);
          Serial.println(msg1[1]);
          Serial.println(msg1[2]);
          Serial.println(msg1[3]);


          lcd.setCursor(0, 0);
          lcd.print("FOMPWAT     ");
          lcd.setCursor(0, 1);
          lcd.print("WAITWAT            ");
          lcd.setCursor(11, 1);
          lcd.print(msg1[3]);

          Mode = 101;


          break;
        }
      }
      if (millis() > TimePlusDelayRecieving && SomeOtherBoolVariable ) {
        Mode = 42; /////Error timeout
      }
      break;
    case 101:
Serial.println("Zustand 101");
      SomeFurtherBoolVariable = true;
      if (SSS7.hasReceived()) {

        SSS7.getReceived(msg1);

        Serial.print("Got data:");
        Serial.println((char*)msg1);

        if ((msg1[0] == MSG_BEGIN_TRANSFER) && (msg1[2] == ID)) {
          SomeFurtherBoolVariable = false;
          Serial.println(msg1[0]);
          Serial.println(msg1[1]);
          Serial.println(msg1[2]);
          Serial.println(msg1[3]);


          lcd.setCursor(0, 0);
          lcd.print("INSERT CAPSULE !!!             ");
          lcd.setCursor(0, 1);
          lcd.print(" FOMPINGWAT                       ");
          while (blinking < 20) {
            blinking++;
            digitalWrite(22, LOW);
            delay(1000);
            digitalWrite(22, HIGH);
            delay(1000);
          }
        }
        lcd.setCursor(0, 0);
        lcd.print("THANKS FOR USING           ");
        lcd.setCursor(0, 1);
        lcd.print("SEIDENSTRASSE           ");

        Mode = 0;
        break;
      }
      if (millis() > TimePlusDelayRecieving) {
        Mode = 42; /////Error timeout
      }

  }
}









