#include "Adafruit_FONA.h"

#define FONA_RX 10
#define FONA_TX 11
#define FONA_RST 6

// this is a large buffer for replies
char replybuffer[255];

#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
//  HardwareSerial *fonaSerial = &Serial1;

// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
// Use this one for FONA 3G
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

uint8_t type;

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 9, 5, 4, 3, 2); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7

int buzzer = 13;

// define ultrasonic sensor pin numbers
const int trigPin = 7; //Echo Pin
const int echoPin = 8; // Trigger Pin

// defines variables
long duration;
int distance;

int maximumRange = 70; // Maximum range needed
int minimumRange = 0; // Minimum range needed

void setup() {

//lcd.display();
  lcd.begin(16, 2);
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
pinMode(buzzer, OUTPUT );

while (!Serial);

  Serial.begin(115200);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  type = fona.type();
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found "));
  switch (type) {
    case FONA800L:
      Serial.println(F("FONA 800L")); break;
    case FONA800H:
      Serial.println(F("FONA 800H")); break;
    case FONA808_V1:
      Serial.println(F("FONA 808 (v1)")); break;
    case FONA808_V2:
      Serial.println(F("FONA 808 (v2)")); break;
    case FONA3G_A:
      Serial.println(F("FONA 3G (American)")); break;
    case FONA3G_E:
      Serial.println(F("FONA 3G (European)")); break;
    default: 
      Serial.println(F("???")); break;
  }
  
  // Print module IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }
delay(1000);
}


void loop() {
 
lcd.setCursor(0, 0);
lcd.print("WELCOME!       ");

// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

//Calculate the distance (in cm) based on the speed of sound.
distance= duration*0.034/2;
Serial.println(distance);
/*
  Serial.print(F("FONA> "));
  while (! Serial.available() ) {
    if (fona.available()) {
      Serial.write(fona.read());
    }
  }
*/
 if (distance <= maximumRange && distance >= minimumRange)
 {
 lcd.setCursor(0, 0);
 lcd.print("Please Wait! ");
 lcd.setCursor(0, 1);
 lcd.print("Sending SMS....."); //Alerting Owner.
 digitalWrite(buzzer, HIGH);
 delay(10000);
 digitalWrite(buzzer, LOW);
 
 sendSMS();
 delay(8000);
/*
 if (fona.available() ){
  receiveSMS();
  delay(3000);
  }
  else{
  */
      lcd.setCursor(0, 0);
      lcd.print("Please!         ");
      lcd.setCursor(0, 1);
      lcd.print("Give me a minute");
      delay(5000);
  
      lcd.setCursor(0, 0);
      lcd.print("Sorry!          ");
      lcd.setCursor(0, 1);
      lcd.print("No one is home  ");
      delay(3000);
/* } */
 }
 else{
      lcd.setCursor(0, 1);
      lcd.print("To a sweet home.");
  }
delay(1000);
}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}

uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}


void sendSMS()
{
   /*** SMS ***/
        // send an SMS!

     String mess="There is someone at the door";
     String phone="+2347036448938";
     Serial.println(phone);
  
        char sendto[phone.length()+1], message[mess.length()+1];
        mess.toCharArray(message,mess.length()+1 );
        phone.toCharArray(sendto,phone.length()+1 );

        flushSerial();
       
        if (!fona.sendSMS(sendto, message)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("Sent!"));
        }

delay(6000);
 }

 void receiveSMS(){

        // read an SMS
        flushSerial();
        Serial.print(F("Read #"));
        uint8_t smsn = readnumber();
        Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);

        // Retrieve SMS sender address/phone number.
        if (! fona.getSMSSender(smsn, replybuffer, 250)) {
          Serial.println("Failed!");
         // break;
        }
        Serial.print(F("FROM: ")); Serial.println(replybuffer);

        // Retrieve SMS value.
        uint16_t smslen;
        if (! fona.readSMS(smsn, replybuffer, 250, &smslen)) { // pass in buffer and max len!
          Serial.println("Failed!");
          //break;
        }
        Serial.print(F("***** SMS #")); Serial.print(smsn);
        Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
        Serial.println(replybuffer);
        Serial.println(F("*****"));

  }
 
