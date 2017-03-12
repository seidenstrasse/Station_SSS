 #include <Arduino.h>
#include "ardusss7.h"
#include "Keypad.h"
#include "ss7content.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>


//#define ID 2


const byte ROWS = 4; //four row
const byte COLS = 4; //four columns
char keys[ROWS][COLS] =
{ {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {
  33,31,29,27
}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  32,30,28,26
}; //connect to the column pinouts of the keypad
int count = 0;
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


LiquidCrystal lcd(38, 36, 52, 50, 48, 46  );
///////////////////////////////////////////////////////
byte Fomp[8] = {
  B11111,
  B00001,
  B00001,
  B01111,
  B00001,
  B00001,
  B00001,
};
byte fomP[8] = {
  B01111,
  B10001,
  B10001,
  B01111,
  B00001,
  B00001,
  B00001,
};
byte hEllo[8] = {
  B11111,
  B00001,
  B00001,
  B11111,
  B00001,
  B00001,
  B111111,
};
byte heLLo[8] = {
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B11111,
};
byte c3[8] = {
  B11111,
  B01000,
  B00100,
  B01000,
  B10000,
  B10001,
  B01110,
};
byte C3[8] = {
  B01110,
  B10001,
  B00001,
  B00001,
  B00001,
  B10001,
  B01110,
};
///////////////////////////////////////////////////////////




unsigned long i;
int blinking;
int state = 1;
char key = -1;
int nkey;
char key1 = -1;
int nkey1;
int dest = -1;
char type = 'Z';
char confirmDest = -1;
unsigned long saveNewMillis;

int site = 0;
unsigned long delaytime = 6000;
unsigned long delaytimesend = 2000;
unsigned long delaytimereceive = 90000;
unsigned long timenow;
bool fail = false;
uint8_t msg[SSS7_PAYLOAD_SIZE];
uint8_t msg1[SSS7_PAYLOAD_SIZE];
char StationNumber = '5';
uint8_t ID;
bool stuff=false;
bool thing=true;
/////////////////////////////////////////////////////////////
void setup()
{
  ID = EEPROM.read(1);
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);

  
  lcd.begin(16, 2);

  Serial.begin(9600);
  Serial.print("init");
  SSS7.init();
  // set up the LCD's number of columns and rows:


  // Print a message to the LCD.

lcd.setCursor(12, 0);

  lcd.print("ID=");
  
lcd.setCursor(15, 0);
  lcd.print(ID);
 

  lcd.createChar(0, Fomp);
  lcd.createChar(1, fomP);
  lcd.createChar(2, hEllo);
  lcd.createChar(3, heLLo);
  lcd.createChar(4, c3);
  lcd.createChar(5, C3);



  lcd.setCursor(0, 0);

  lcd.print("H   O     !");
  lcd.setCursor(1, 0);
  lcd.write(byte(2));
  lcd.setCursor(2, 0);
  lcd.write(byte(3));
  lcd.setCursor(3, 0);
  lcd.write(byte(3));

  lcd.setCursor(6, 0);
  lcd.write(byte(4));
  lcd.setCursor(7, 0);
  lcd.write(byte(4));
  lcd.setCursor(8, 0);
  lcd.write(byte(5));
  lcd.setCursor(9, 0);
  lcd.write(byte(4));


  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.print("OM !!");
  lcd.setCursor(3, 0);
  lcd.write(byte(1));


  delay(2000);


}






/////////////////////////////////////////////////////////////
void loop()
{
  switch (state) {
    case 0: timenow = millis() + delaytime;
    case 1:  // ASK FOR SEND ALTERNATIVES

      //Anzeige Text
      if (site == 0) {
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HOW to SEND         ");
        lcd.setCursor(0, 1);
        lcd.print("PRESS:           ");
      }

      if (site == 1) {
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("A FOR DATAMATRIX    ");
        lcd.setCursor(0, 1);
        lcd.print("B FOR ADDRESS       ");
      }
      if (site == 2) {
        lcd.setCursor(0, 0);
        lcd.print("FOR HELP VISIT        ");
        lcd.setCursor(0, 1);
        lcd.print("SEIDENSTRASSE        ");
      }

      type = keypad.getKey();

      if (type != NO_KEY) {

        if (type == 'A') {
          state = 6;
          site = 0;
          break;
        }

        if (type == 'B') {
          state = 2;
          site = 0;
          break;
        }

      }
      else {
        if (timenow < millis()) {
          site++;
          if (site == 3)site = 0;

          state = 0;
          break;
          delay(100);
        }

      }
      delay(100);
      state = 1;
      break;

    case 2: //INSERT DESTINATION
      lcd.clear();
      key = -1;
      key1 = -1;
      dest = -1;

      lcd.print("DESTINATION: ##   ");


      lcd.setCursor(13, 0);
      lcd.blink();

      do {
        key = keypad.waitForKey();
      } while (key == '#' || key == '*' || key == 'A' || key == 'B' || key == 'C' || key == 'D');


      lcd.print(key);



      lcd.setCursor(14, 0);
      lcd.blink();
      do {
        key1 = keypad.waitForKey();
      } while (key1 == '#' || key1 == '*' || key1 == 'A' || key1 == 'B' || key1 == 'C' || key1 == 'D' );



      lcd.print(key1);




      nkey = (int)key;
      nkey1 = (int)key1;
      dest = (nkey - 48) * 10 + (nkey1 - 48);
      lcd.noBlink();

      lcd.setCursor(0, 1);
      lcd.print(" # TO CONFIRM         ");

      if (keypad.waitForKey() == '#') {
        lcd.clear();
        lcd.print((int)dest);
        state = 7;
        break;
      }

      state = 0;

      break;


    //////////////////////////////////////////////////////////////////////////////
    case 6: //DATAMATRIX SEND DIALOG
      lcd.clear();

      lcd.print("CODE ON CAPSULE?     ");
      lcd.setCursor(0, 1);
      lcd.print(" # TO CONFIRM        ");

      confirmDest = keypad.waitForKey();
      if (confirmDest == '#') {
        dest = -1;
        state = 7;
      } else {
        state = 0;
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


      state = 99;
      break;

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



      state = 0;
      break;





    case 99://///////////////////////////////////////////////////////////
      timenow = millis() + delaytimesend;

      do {
         delay(800);
stuff=false;
       
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
          stuff=SSS7.sendFailed();
          Serial.println("Finished Sending");
         // break;
       
          if (stuff) {
            Serial.println("Send failed");
            state = 42;
          }
          
          42
        } else {
      stuff=false;
          msg[0] = MSG_REQUEST_DIRECT;
          msg[1] = ID;
          msg[2] = ID_COORDINATOR;
          msg[3] = dest;

          Serial.println("Waiting to send");
          while (!SSS7.canSend());
          Serial.println("Sending ...");
          SSS7.send(msg);
          
          while (!SSS7.canSend());
          stuff=SSS7.sendFailed();
          Serial.println("Finished Sending");
         // break;
         
       
          if (stuff) {
            Serial.println("Send failed");
            state = 42;
            
          }

        }
        
       
     } while (millis() < timenow && stuff);




      state = 100;
      //state=0;
      break;



    case 100://////////////////////////////////////////////////////////////
      timenow = millis() + delaytimereceive;

      do {
        thing=true;
        delay(200);
        if (SSS7.hasReceived()) {
          uint8_t msg1[SSS7_PAYLOAD_SIZE];
          SSS7.getReceived(msg1);

          Serial.print("Got data:");
          Serial.println((char*)msg1);
          if ((msg1[0] == MSG_CONFIRM_REQUEST) && (msg1[2] == ID)) {
            thing=false;
            Serial.println(msg1[0]);
            Serial.println(msg1[1]);
            Serial.println(msg1[2]);
            Serial.println(msg1[3]);

            
            lcd.setCursor(0, 0);
            lcd.print("WORKING FOR YOU        ");
            lcd.setCursor(0, 1);
            lcd.print("PLEASE WAIT            ");
            lcd.setCursor(11, 1);
            lcd.print(msg1[3]);

            state = 101;
            break;
          }
        }
      } while (millis() < timenow && thing );

      state = 42; /////Error timeout
     break;
    case 101:
      timenow = millis() + delaytimereceive;

      do {
        thing=true;
        if (SSS7.hasReceived()) {

          SSS7.getReceived(msg1);

          Serial.print("Got data:");
          Serial.println((char*)msg1);

          if ((msg1[0] == MSG_BEGIN_TRANSFER) && (msg1[2] == ID)) {
            thing=false;
            Serial.println(msg1[0]);
            Serial.println(msg1[1]);
            Serial.println(msg1[2]);
            Serial.println(msg1[3]);


            lcd.setCursor(0, 0);
            lcd.print("INSERT CAPSULE !!!             ");
            lcd.setCursor(0, 1);
            lcd.print(" WE WORK FOR YOU                       ");
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
          delay(1000);
          state = 0;
          break;
        }
      } while (millis() < timenow);

      state = 42; /////Error timeout
//      break;
  }
}


  



