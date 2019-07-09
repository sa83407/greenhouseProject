//Replaced BATTERY ON Real TIME CLOCK 4/2019
//Comment out interrupt sections if uploading without adding power controlling hardware


//include libraries needed, intialize variables, definitions
#include <Wire.h>
#include "Wire.h"
#include "Adafruit_HTU21DF.h"
#include <SPI.h>
#include <SoftwareSerial.h>
extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
#include "Adafruit_FONA.h"
int textingsignal = 1;
#define FONA_RX 10
#define FONA_TX 11
#define FONA_RST 5



#include <OLED_I2C.h>
#include <SD.h>
#include <DS1302.h>
DS1302 rtc(32, 33, 34);
Time t;
OLED  myOLED(SDA, SCL, 8);
const int chipSelect = 53;
File myFile;
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

// this is a large buffer for replies
char replybuffer[255];

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines
// and uncomment the HardwareSerial line
//#include <SoftwareSerial.h>
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
#define TCAADDR 0x70 // mux 1 #define TCAADDR 0x70 // mux 1
#define TCAADDR2 0x71 // mux 2 #define TCAADDR 0x71 // mux 2
#define TCAADDR3 0x72 // mux 3 #define TCAADDR 0x72 // mux 3
#define TCAADDR4 0x73 // mux 4 #define TCAADDR 0x73 // mux 4
// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (A5 on UNO)
// Connect SDA to I2C data pin (A4 on UNO)

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
// array of temperature designated by [ ] when calling call from 0-9 for an array of size 10
float temperature[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float humidity[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float humiditymax[10] = {60.0, 60.0, 60.0, 60.0, 60.0, 60.0, 60.0, 60.0, 60.0, 60.0};
float humiditymin[10] = {24.0, 24.0, 24.0, 24.0, 24.0, 24.0, 24.0,  24.0, 24.0, 24.0};
//temperature max is 100F and min is 60F
float temperaturemax[10] = {38.00, 38.00, 38.00, 38.00, 38.00, 38.00, 38.00, 38.00, 38.00, 38.00};
float temperaturemin[10] = {15.5, 15.5, 0, 15.5, 0,0 , 0, 15.5, 15.5, 0};
//temp below  60 above 100


//LOG DATA
String stringCurrMon, stringCurrDate, stringSavedMon, stringSavedDate;
char currMon, savedMon, combineDate;
char currDate, savedDate,currYear, savedYear;
char fileName[] = "00000000.txt";
char dumpName[] = "00000000.txt";





int heart = 12;
const int heartwrite = 49;
int printtofile = 0;

//if temp too high/low, send notification every 12 hrs(in millis) until resolved
const unsigned long lowTempNotification = 43200000;
const unsigned long highTempNotification = 43200000;
const unsigned long batteryBackupNotification = 43200000;
unsigned long startTimeHigh;
unsigned long currentTimeHigh;
unsigned long startTimeLow;
unsigned long currentTimeLow;
unsigned long startTimeBattery;
unsigned long currentTimeBattery;
int lastCheckHigh = 0;
int lastCheckLow = 0;
//reference voltage(5V) and pin battery is connected to (A0)
const int batteryPin = 0;
const float referenceVolts = 5.0;
//power ON/OFF flag
int powerDown = 0;
int loopCount = 0;

int sendFirstHighNotification = 0;
int sendFirstLowNotification = 0;
unsigned long currentTime;
unsigned long previousTimeLow;
unsigned long previousTimeHigh;
const unsigned long sendNotificationHigh = 3600000; //every 1 hour
const unsigned long sendNotificationLow = 3600000; //every 1 hour



void heartbeat() {
  digitalWrite(heart, HIGH);
  delay(15);
  digitalWrite(heart, LOW);
  delay(15);
}
void heartwriter() {
  digitalWrite(heartwrite, HIGH);
  delay(15);
  digitalWrite(heartwrite, LOW);
  delay(15);
}

//Variable will be adjusted in interrupt cylce
bool powerOut = false;
int powerCount = 0;
//Interrupt if power goes low
void POWER_LOW();
//Interrupt if power goes back high
void POWER_HIGH();

void checkConditions(float humidity[], float temperature[], float humiditymax[], float humiditymin[],
                     float temperaturemax[], float temperaturemin[]);
void getFileName();

void voltageMonitor();

void setup() {

  //Use pin 2 for power outage interrupts
  //attachInterrupt(digitalPinToInterrupt(2), POWER_LOW(), LOW);
  //FIGURE OUT HOW TO DO POWER BACK HIGH
  //attachInterrupt(digitalPinToInterrupt(2), POWER_HIGH(), RISING);
 
  Wire.begin(); // Initiate the Wire library
  Serial.begin(9600);
  tcaselect(7);
  tcaselect2(7);

//  Serial.println("Begin");
  htu.begin();

  Serial.print("Initializing SD card...");
//  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
//  // Note that even if it's not used as the CS pin, the hardware SS pin
//  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
//  // or the SD library functions will not work.
  pinMode(SS, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

//  rtc.halt(false);
//  rtc.writeProtect(false);
 
//            // The following lines can be commented out to use the values already stored in the DS1302
//              rtc.setDOW(TUESDAY);        // Set Day-of-Week to FRIDAY
//              rtc.setTime(18, 15, 0);     // Set the time to 12:00:00 (24hr format)
//              rtc.setDate(30, 4, 2019);   // Set the date to August 6th, 2010 day/month/year
 
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
  //
  //          // Print module IMEI number.
  char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
  fona.setGPRSNetworkSettings(F("hologram"), F("your username"), F("your password"));

  // Optionally configure HTTP gets to follow redirects over SSL.
  // Default is not to follow SSL redirects, however if you uncomment
  // the following line then redirects over SSL will be followed.
  //fona.setHTTPSRedirect(true);


  ///
  pinMode(heart, OUTPUT);
  pinMode(heartwrite, OUTPUT);
  Wire.begin();

  delay(10);
  // Serial.println("\ndone");
  startTimeHigh = millis();
  startTimeLow = millis();

}

void loop() {
  t = rtc.getTime();
  loopCount++;
  currentTime = millis();

  if(loopCount >= 5000){
    loopCount = 0;
  }
 
  currentTimeHigh = millis();
  currentTimeLow = millis();
  ///////////////send data from sensors to the lath house , main room, green house 1 and 2 ////////////
  // Ex. tcaselect(1) is option 1 temperature/humidity reading, tcaselect(2) is option 2  temperature/humidity reading..etc
  //total 10 options/readings, mux1 reads from sensors 0-3, mux2 reads from sensors 4-7, mux3 reads from sensors 8 & 9

  //mux1 GH1
 tcaselect(0);
  temperature[0] = htu.readTemperature();
  humidity[0] = htu.readHumidity();
  delay(100);

  //GH2
  tcaselect(1);
  temperature[1] = htu.readTemperature();
  humidity[1] = htu.readHumidity();
  delay(100);
 
//  //GH3
  tcaselect(2);
  temperature[2] = htu.readTemperature();
  humidity[2] = htu.readHumidity();
  delay(100);

//  //GH4
  tcaselect(3);
  temperature[3] = htu.readTemperature();
  humidity[3] = htu.readHumidity();
  delay(100);

//  //HH
  tcaselect(4);
  temperature[4] = htu.readTemperature();
  humidity[4] = htu.readHumidity();
  delay(100);

//  //ISO1
  tcaselect2(0);
  temperature[5] = htu.readTemperature();
  humidity[5] = htu.readHumidity();
  delay(100);
 
//  //ISO2
  tcaselect2(1);
  temperature[6] = htu.readTemperature();
  humidity[6] = htu.readHumidity();
  delay(100);
 
//  //ISO3
  tcaselect2(2);
  temperature[7] = htu.readTemperature();
  humidity[7] = htu.readHumidity();
  delay(100);
 
//  //ISO4
  tcaselect2(3);
  temperature[8] = htu.readTemperature();
  humidity[8] = htu.readHumidity();
  delay(100);

//  //LH
  tcaselect2(4);
  temperature[9] = htu.readTemperature();
  humidity[9] = htu.readHumidity();
  delay(100);



  //print data to serial monitor
  //GH1
  Serial.print((int) ((temperature[0] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //GH2
  Serial.print((int) ((temperature[1] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //GH3
  Serial.print((int) ((temperature[2] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //GH4
  Serial.print((int) ((temperature[3] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //HH
  Serial.print((int) ((temperature[4] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //IS1
  Serial.print((int) ((temperature[5] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //ISO2
  Serial.print((int) ((temperature[6] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //ISO3
  Serial.print((int) ((temperature[7] * 9.0) / 5.0 + 32.0));
  Serial.print(",");
  //ISO4
  Serial.print((int) ((temperature[8] * 9.0) / 5.0 + 32));
  Serial.print(",");
  //LH
  Serial.print((int) ((temperature[9] * 9.0) / 5.0 + 32));
  Serial.print(",");



  Serial.print((int) humidity[0]);
  Serial.print(",");
  Serial.print((int) humidity[1]);
  Serial.print(",");
  Serial.print((int) humidity[2]);
  Serial.print(",");
  Serial.print((int) humidity[3]);
  Serial.print(",");

  Serial.print((int) humidity[4]);
  Serial.print(",");
  Serial.print((int) humidity[5]);
  Serial.print(",");
  Serial.print((int) humidity[6]);
  Serial.print(",");
  Serial.print((int) humidity[7]);
  Serial.print(",");

  Serial.print((int) humidity[8]);
  Serial.print(",");
  Serial.print((int) humidity[9]);
  Serial.print(",");
  Serial.println(loopCount);

//check date and time logging into sd card
//Serial.println(rtc.getTimeStr());
//Serial.println(rtc.getDateStr());
//Serial.println(rtc.getDOWStr());
//Serial.println(t.date);
//Serial.println(rtc.getMonthStr());


stringCurrMon=t.mon;
stringCurrDate=t.date;
currMon =t.mon;
currDate=t.date;



if( (t.min==00) ||(t.min==15) || (t.min==30) || (t.min==45)){
 


  sprintf(fileName, "%02d%02d19.txt",currMon,currDate);
  Serial.println(fileName);

                  if( (stringCurrMon==stringSavedMon) && (stringCurrDate==stringSavedDate) ){
                    //save data to text file
                    //Serial.println("in logging data");
                         myFile = SD.open(fileName, FILE_WRITE);
                               if (myFile) {
          
          
                                      myFile.println((int) ((temperature[0] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[1] * 9.0) / 5.0 + 32));
                                      delay(100);      
                                      myFile.println((int) ((temperature[2] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[3] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[4] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[5] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[6] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[7] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[8] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[9] * 9.0) / 5.0 + 32));
                                      delay(100);
                                  
                  
                        
                                      myFile.println(int(humidity[0]));
                                      delay(100);
                                      myFile.println(int(humidity[1]));
                                      delay(100);
                                      myFile.println(int(humidity[2]));
                                      delay(100);
                                      myFile.println(int(humidity[3]));
                                      delay(100);
                                      myFile.println(int(humidity[4]));
                                      delay(100);
                                      myFile.println(int(humidity[5]));
                                      delay(100);
                                      myFile.println(int(humidity[6]));
                                      delay(100);
                                      myFile.println(int(humidity[7]));
                                      delay(100);
                                      myFile.println(int(humidity[8]));
                                      delay(100);
                                      myFile.println(int(humidity[9]));
                                      delay(100);
                
                
          
                          // close the file:
                            myFile.close();
                              }else {
                                // if the file didn't open, print an error:
                                Serial.println("error opening text ile");
                                    }
                  }
                  else{
                  //  //create text file
                  //  //then save data to NEW text file
                  //Serial.println("creating txt file");
                  
                  stringSavedMon=stringCurrMon;
                  stringSavedDate=stringCurrDate;
               
                       myFile = SD.open(fileName, FILE_WRITE);
              
                        // if the file opened okay, write to it:
                        if (myFile) {
          
          
                                      myFile.println((int) ((temperature[0] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[1] * 9.0) / 5.0 + 32));
                                      delay(100);      
                                      myFile.println((int) ((temperature[2] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[3] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[4] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[5] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[6] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[7] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[8] * 9.0) / 5.0 + 32));
                                      delay(100);
                                      myFile.println((int) ((temperature[9] * 9.0) / 5.0 + 32));
                                      delay(100);
                                  
                  
                        
                                      myFile.println(int(humidity[0]));
                                      delay(100);
                                      myFile.println(int(humidity[1]));
                                      delay(100);
                                      myFile.println(int(humidity[2]));
                                      delay(100);
                                      myFile.println(int(humidity[3]));
                                      delay(100);
                                      myFile.println(int(humidity[4]));
                                      delay(100);
                                      myFile.println(int(humidity[5]));
                                      delay(100);
                                      myFile.println(int(humidity[6]));
                                      delay(100);
                                      myFile.println(int(humidity[7]));
                                      delay(100);
                                      myFile.println(int(humidity[8]));
                                      delay(100);
                                      myFile.println(int(humidity[9]));
                                      delay(100);
                
          
                          // close the file:
                            myFile.close();
                              }else {
                                // if the file didn't open, print an error:
                                Serial.println("error opening txt file");
                                    }
                  }
        //delay 50 seconds to prevent duplicate records
        delay(50000);
}
  //  //display in each OLED
  //  //mux1 GH1
  tcaselect(0);
  myOLED.begin();
  myOLED.setFont(SmallFont); //font size
  myOLED.print("F1", LEFT, 0);
  myOLED.print("Temperature1: ", LEFT, 24);//labels displayed on screen, Ex. "Temperature1: "
  myOLED.print("Humidity1: ", LEFT, 48); // Ex. "Humidity1: "
  myOLED.update();
  delay(200);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[0], 2, RIGHT, 40); //display data next to labels, temperature & humidity
  myOLED.update();
  myOLED.setFont(BigNumbers);
  //fahrenheit conversion, for celsius just delete conversion
  myOLED.printNumF((temperature[0] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(200);

  //GH2
  tcaselect(1);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[1], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[1] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);

//  //GH3
  tcaselect(2);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[2], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[2] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);
//
//  //GH4
  tcaselect(3);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[3], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[3] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);
//
//  //HH
  tcaselect(4);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[4], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[4] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);
//
//
//
//  //ISO1
  tcaselect2(0);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[5], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[5] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);
//
//  //ISO2
  tcaselect2(1);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[6], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[6] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);
//
//  
//  //ISO3
  tcaselect2(2);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[7], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[7] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);
//  
//  //ISO4
  tcaselect2(3);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[8], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[8] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);
//
//    //LH
  tcaselect2(4);
  myOLED.begin();
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print("F2", LEFT, 0);
  myOLED.print("Temperature2 ", LEFT, 24);
  myOLED.print("Humidity2 ", LEFT, 48);
  myOLED.update();
  delay(300);
  myOLED.setFont(BigNumbers);
  myOLED.printNumF(humidity[9], 2, RIGHT, 40);
  myOLED.update();
  myOLED.setFont(BigNumbers);
  myOLED.printNumF((temperature[9] * 9.0) / 5.0 + 32.0, 2, RIGHT, 0);
  myOLED.update();
  delay(300);

//90 loops * @ approx. 10 seconds each loop = every 20 minutes log data

//Change to much lower to test SD card writing
//Was previously 90


if(printtofile == 90){
 
      sensorwritesd();
 
    printtofile=0;
}
printtofile++;
//Serial.println(printtofile);  

 //Used to text if power is low
powerCount++;
if(powerCount == 31200){
  if(powerOut == true)
    POWER_LOW();
  powerCount = 0;
  }
  delay(4000);
  //Serial.flush();
if((currentTime-previousTimeHigh) > sendNotificationHigh){
  for (int i = 0; i < 10; i++) { //for sensors 1-10
    if (temperature[i] > temperaturemax[i]) {
      
      
      //if temp sensor 1 is high send sms
      if (i == 0) {
        Serial.println("WARNING TEMP SENSOR 1 HIGH");
        char sendto[12], message[141];
                                            flushSerial();
                                            //Serial.print(F("Send to #"));
                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
        
                                            message[0] = 'W';
                                            message[1] = 'A';
                                            message[2] = 'R';
                                            message[3] = 'N';
                                            message[4] = 'I';
                                            message[5] = 'N';
                                            message[6] = 'G';
                                            message[7] = ' ';
                                            message[8] = 'G';
                                            message[9] = 'H';
                                            message[10] = '1';
                                            message[11] = ' ';
                                            message[12] = 'T';
                                            message[13] = 'E';
                                            message[14] = 'M';
                                            message[15] = 'P';
                                            message[16] = ' ';
                                            message[17] = 'H';
                                            message[18] = 'I';
                                            message[19] = 'G';
                                            message[20] = 'H';
                                            message[21] = '\0';

                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
        
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
        textingsignal = 0;
        delay(100);
      }
      //if temp sensor 2 is high send sms
      if (i == 1) {
        Serial.println("WARNING TEMP SENSOR 2 HIGH");
        char sendto[12], message[141];
                                         flushSerial();
                                    //    Serial.print(F("Send to #"));

                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
                                            message[0] = 'W';
                                            message[1] = 'A';
                                            message[2] = 'R';
                                            message[3] = 'N';
                                            message[4] = 'I';
                                            message[5] = 'N';
                                            message[6] = 'G';
                                            message[7] = ' ';
                                            message[8] = 'G';
                                            message[9] = 'H';
                                            message[10] = '2';
                                            message[11] = ' ';
                                            message[12] = 'T';
                                            message[13] = 'E';
                                            message[14] = 'M';
                                            message[15] = 'P';
                                            message[16] = ' ';
                                            message[17] = 'H';
                                            message[18] = 'I';
                                            message[19] = 'G';
                                            message[20] = 'H';
                                            message[21] = '\0';
                                         //   
                                    
                                            if (fona.sendSMS(sendto, message)) {
                                              Serial.println(F("sent"));
                                    
                                            }
                                            else {
                                              Serial.println(F("Sent anyway!"));
                                            }
                                            textingsignal = 0;
                                             delay(100);
                                    
      }
      //                    //if temp sensor 3 is high send sms
                                              if(i==2){
//                                                        Serial.println("WARNING TEMP SENSOR 3 HIGH");
//      
//                                                      char sendto[12], message[141];
//                                                        flushSerial();
//                                           //        Serial.print(F("Send to #"));
//      
//                                                      sendto[0]='1';
//                                                      sendto[1]='3';
//                                                      sendto[2]='1';
//                                                      sendto[3]='0';
//                                                      sendto[4]='9';
//                                                      sendto[5]='9';
//                                                      sendto[6]='4';
//                                                      sendto[7]='7';
//                                                      sendto[8]='1';
//                                                      sendto[9]='6';
//                                                      sendto[10]='9';
//                                                      sendto[11]='\0';
//
//                                                      message[0] = 'W';
//                                                      message[1] = 'A';
//                                                      message[2] = 'R';
//                                                      message[3] = 'N';
//                                                      message[4] = 'I';
//                                                      message[5] = 'N';
//                                                      message[6] = 'G';
//                                                      message[7] = ' ';
//                                                      message[8] = 'G';
//                                                      message[9] = 'H';
//                                                      message[10] = '3';
//                                                      message[11] = ' ';
//                                                      message[12] = 'T';
//                                                      message[13] = 'E';
//                                                      message[14] = 'M';
//                                                      message[15] = 'P';
//                                                      message[16] = ' ';
//                                                      message[17] = 'H';
//                                                      message[18] = 'I';
//                                                      message[19] = 'G';
//                                                      message[20] = 'H';
//                                                      message[21] = '\0';
//                                                 //     
//                                                   
//                                                      if (fona.sendSMS(sendto, message)) {
//                                                        Serial.println(F("sent"));
//                                              
//                                                      }
//                                                      else {
//                                                        Serial.println(F("Sent anyway!"));
//                                                      }
//                                                      textingsignal = 0;
//                                                       delay(100);
      
                                              }
                           //if temp sensor 4 is high send sms
      if (i == 3) {
        Serial.println("WARNING TEMP SENSOR 4 HIGH");

        char sendto[12], message[141];
                                              flushSerial();

                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
                                            message[0] = 'W';
                                            message[1] = 'A';
                                            message[2] = 'R';
                                            message[3] = 'N';
                                            message[4] = 'I';
                                            message[5] = 'N';
                                            message[6] = 'G';
                                            message[7] = ' ';
                                            message[8] = 'G';
                                            message[9] = 'H';
                                            message[10] = '4';
                                            message[11] = ' ';
                                            message[12] = 'T';
                                            message[13] = 'E';
                                            message[14] = 'M';
                                            message[15] = 'P';
                                            message[16] = ' ';
                                            message[17] = 'H';
                                            message[18] = 'I';
                                            message[19] = 'G';
                                            message[20] = 'H';
                                            message[21] = '\0';
                                     //       ;
                                            
                                            if (fona.sendSMS(sendto, message)) {
                                              Serial.println(F("sent"));
                                    
                                            }
                                            else {
                                              Serial.println(F("Sent anyway!"));
                                            }
                                            textingsignal = 0;
                                             delay(100);
        
      }
      //                      //if temp sensor 5 is high send sms
                                              if(i==4){
//                                                        Serial.println("WARNING TEMP SENSOR 5 HIGH");
//      //            //                            //       // send an SMS!
//                                                        char sendto[12], message[141];
//                                                      flushSerial();
//      
//                                                      sendto[0]='1';
//                                                      sendto[1]='3';
//                                                      sendto[2]='1';
//                                                      sendto[3]='0';
//                                                      sendto[4]='9';
//                                                      sendto[5]='9';
//                                                      sendto[6]='4';
//                                                      sendto[7]='7';
//                                                      sendto[8]='1';
//                                                      sendto[9]='6';
//                                                      sendto[10]='9';
//                                                      sendto[11]='\0';
//                  
//                                            message[0] = 'W';
//                                            message[1] = 'A';
//                                            message[2] = 'R';
//                                            message[3] = 'N';
//                                            message[4] = 'I';
//                                            message[5] = 'N';
//                                            message[6] = 'G';
//                                            message[7] = ' ';
//                                            message[8] = 'H';
//                                            message[9] = 'H';
//                                            message[10] = ' ';
//                                            message[11] = 'T';
//                                            message[12] = 'E';
//                                            message[13] = 'M';
//                                            message[14] = 'P';
//                                            message[15] = ' ';
//                                            message[16] = 'H';
//                                            message[17] = 'I';
//                                            message[18] = 'G';
//                                            message[19] = 'H';
//                                            message[20] = '\0';
//                                                 //     
//                                             
//                                                      if (fona.sendSMS(sendto, message)) {
//                                                        Serial.println(F("sent"));
//                                              
//                                                      }
//                                                      else {
//                                                        Serial.println(F("Sent anyway!"));
//                                                      }
//                                                      textingsignal = 0;
//                                                       delay(100);
      
                                              }
      //                  //                      //if temp sensor 6 is high send sms
                                              if(i==5){
                                                        Serial.println("WARNING TEMP SENSOR 6 HIGH");
      //            //                            //       // send an SMS!
                                                        char sendto[12], message[141];
                                                      flushSerial();
                                             
                                                      
                                                      sendto[0]='1';
                                                      sendto[1]='3';
                                                      sendto[2]='1';
                                                      sendto[3]='0';
                                                      sendto[4]='9';
                                                      sendto[5]='9';
                                                      sendto[6]='4';
                                                      sendto[7]='7';
                                                      sendto[8]='1';
                                                      sendto[9]='6';
                                                      sendto[10]='9';
                                                      sendto[12]='\0';
                  
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'I';
                                                      message[9] = 'S';
                                                      message[10] = 'O';
                                                      message[11] = '1';
                                                      message[12] = ' ';
                                                      message[13] = 'T';
                                                      message[14] = 'E';
                                                      message[15] = 'M';
                                                      message[16] = 'P';
                                                      message[17] = ' ';
                                                      message[18] = 'H';
                                                      message[19] = 'I';
                                                      message[20] = 'G';
                                                      message[21] = 'H';
                                                      message[22] = '\0';
                                                 //     
                                                      
                                                      if (fona.sendSMS(sendto, message)) {
                                                        Serial.println(F("sent"));
                                              
                                                      }
                                                      else {
                                                        Serial.println(F("Sent anyway!"));
                                                      }
                                                      textingsignal = 0;
                                                       delay(100);
      
                                              }
      ////                      //if temp sensor 7 is high send sms
                                             if(i==6){
//                                                        Serial.println("WARNING TEMP SENSOR 7 HIGH");
//      //          //                            //       // send an SMS!
//                                                      char sendto[12], message[141];
//                                                    flushSerial();
//      //          //                            //        Serial.print(F("Send to #"));
//                                                  //  
//                                                    sendto[0]='1';
//                                                    sendto[1]='3';
//                                                    sendto[2]='1';
//                                                    sendto[3]='0';
//                                                    sendto[4]='9';
//                                                    sendto[5]='9';
//                                                    sendto[6]='4';
//                                                    sendto[7]='7';
//                                                    sendto[8]='1';
//                                                    sendto[9]='6';
//                                                    sendto[10]='9';
//                                                    sendto[11]='\0';
//                
//                                                      message[0] = 'W';
//                                                      message[1] = 'A';
//                                                      message[2] = 'R';
//                                                      message[3] = 'N';
//                                                      message[4] = 'I';
//                                                      message[5] = 'N';
//                                                      message[6] = 'G';
//                                                      message[7] = ' ';
//                                                      message[8] = 'I';
//                                                      message[9] = 'S';
//                                                      message[10] = 'O';
//                                                      message[11] = '2';
//                                                      message[12] = ' ';
//                                                      message[13] = 'T';
//                                                      message[14] = 'E';
//                                                      message[15] = 'M';
//                                                      message[16] = 'P';
//                                                      message[17] = ' ';
//                                                      message[18] = 'H';
//                                                      message[19] = 'I';
//                                                      message[20] = 'G';
//                                                      message[21] = 'H';
//                                                      message[22] = '\0';
//                                              //      
//                                                    
//                                                    if (fona.sendSMS(sendto, message)) {
//                                                      Serial.println(F("sent"));
//                                            
//                                                    }
//                                                    else {
//                                                      Serial.println(F("Sent anyway!"));
//                                                    }
//                                                    textingsignal = 0;
//                                                     delay(100);
      
                                             }
      //                //                      //if temp sensor 8 is high send sms
                                            if(i==7){
                                                      Serial.println("WARNING TEMP SENSOR 8 HIGH");
      //          //                                   // send an SMS!
                                                      char sendto[12], message[141];
                                                    flushSerial();
      //          //                            //        Serial.print(F("Send to #"));
                                              //      
                                                    sendto[0]='1';
                                                    sendto[1]='3';
                                                    sendto[2]='1';
                                                    sendto[3]='0';
                                                    sendto[4]='9';
                                                    sendto[5]='9';
                                                    sendto[6]='4';
                                                    sendto[7]='7';
                                                    sendto[8]='1';
                                                    sendto[9]='6';
                                                    sendto[10]='9';
                                                    sendto[11]='\0';
                
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'I';
                                                      message[9] = 'S';
                                                      message[10] = 'O';
                                                      message[11] = '3';
                                                      message[12] = ' ';
                                                      message[13] = 'T';
                                                      message[14] = 'E';
                                                      message[15] = 'M';
                                                      message[16] = 'P';
                                                      message[17] = ' ';
                                                      message[18] = 'H';
                                                      message[19] = 'I';
                                                      message[20] = 'G';
                                                      message[21] = 'H';
                                                      message[22] = '\0';
                                              //      
                                                   
                                                    if (fona.sendSMS(sendto, message)) {
                                                      Serial.println(F("sent"));
                                            
                                                    }
                                                    else {
                                                      Serial.println(F("Sent anyway!"));
                                                    }
                                                    textingsignal = 0;
                                                     delay(100);
      
                                            }
      //                //                      //if temp sensor 9 is high send sms
                                            if(i==8){
                                                      Serial.println("WARNING TEMP SENSOR 9 HIGH");
      //          //                            //       // send an SMS!
                                                    char sendto[12], message[141];
                                                    flushSerial();
                                            //        Serial.print(F("Send to #"));
                                             //       
                                                    sendto[0]='1';
                                                    sendto[1]='3';
                                                    sendto[2]='1';
                                                    sendto[3]='0';
                                                    sendto[4]='9';
                                                    sendto[5]='9';
                                                    sendto[6]='4';
                                                    sendto[7]='7';
                                                    sendto[8]='1';
                                                    sendto[9]='6';
                                                    sendto[10]='9';
                                                    sendto[11]='\0';
                
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'I';
                                                      message[9] = 'S';
                                                      message[10] = 'O';
                                                      message[11] = '4';
                                                      message[12] = ' ';
                                                      message[13] = 'T';
                                                      message[14] = 'E';
                                                      message[15] = 'M';
                                                      message[16] = 'P';
                                                      message[17] = ' ';
                                                      message[18] = 'H';
                                                      message[19] = 'I';
                                                      message[20] = 'G';
                                                      message[21] = 'H';
                                                      message[22] = '\0';
                                             //       
                                                    
                                                    if (fona.sendSMS(sendto, message)) {
                                                      Serial.println(F("sent"));
                                            
                                                    }
                                                    else {
                                                      Serial.println(F("Sent anyway!"));
                                                    }
                                                    textingsignal = 0;
                                                     delay(100);
      
                                            }
      //                //                      //if temp sensor 10 is high send sms
                                            if(i==9){
                                            //          Serial.println("WARNING TEMP SENSOR 10 HIGH");
                //                            //       // send an SMS!
                                                    char sendto[12], message[141];
                                                    flushSerial();
      //          //                            //        Serial.print(F("Send to #"));
                                             //       
                                                    sendto[0]='1';
                                                    sendto[1]='3';
                                                    sendto[2]='1';
                                                    sendto[3]='0';
                                                    sendto[4]='9';
                                                    sendto[5]='9';
                                                    sendto[6]='4';
                                                    sendto[7]='7';
                                                    sendto[8]='1';
                                                    sendto[9]='6';
                                                    sendto[10]='9';
                                                    sendto[11]='\0';
                               
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'L';
                                                      message[9] = 'H';
                                                      message[10] = ' ';
                                                      message[11] = 'T';
                                                      message[12] = 'E';
                                                      message[13] = 'M';
                                                      message[14] = 'P';
                                                      message[15] = ' ';
                                                      message[16] = 'H';
                                                      message[17] = 'I';
                                                      message[18] = 'G';
                                                      message[19] = 'H';
                                                      message[20] = '\0';
                                          //          
                                                   
                                                    if (fona.sendSMS(sendto, message)) {
                                                      Serial.println(F("sent"));
                                            
                                                    }
                                                    else {
                                                      Serial.println(F("Sent anyway!"));
                                                    }
                                                    textingsignal = 0;
 delay(100);
                
                                      }
      
      
       if(currentTime -previousTimeHigh < 0){
                      previousTimeHigh = 0;
                    }
    }
  }previousTimeHigh = currentTime;
}

//Serial.println(currentTime);
//Serial.println(previousTimeLow);
//Serial.println(sendNotificationLow);

  if((currentTime - previousTimeLow) > sendNotificationLow){
 
  for (int i = 0; i < 10; i++) {
    // Serial.println("in loop");
    if (temperature[i] < temperaturemin[i]) {
      
if (i == 0) {
        Serial.println("WARNING TEMP SENSOR 1 LOW");
        //                                   // send an SMS!
        char sendto[12], message[141];
                                            flushSerial();
                                            
                                          //  
                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
                                                        message[0] = 'W';
                                                        message[1] = 'A';
                                                        message[2] = 'R';
                                                        message[3] = 'N';
                                                        message[4] = 'I';
                                                        message[5] = 'N';
                                                        message[6] = 'G';
                                                        message[7] = ' ';
                                                        message[8] = 'G';
                                                        message[9] = 'H';
                                                        message[10] = '1';
                                                        message[11] = ' ';
                                                        message[12] = 'T';
                                                        message[13] = 'E';
                                                        message[14] = 'M';
                                                        message[15] = 'P';
                                                        message[16] = ' ';
                                                        message[17] = 'L';
                                                        message[18] = 'O';
                                                        message[19] = 'W';
                                                        message[20] = '\0';
                                        //    
                                            
                                            if (fona.sendSMS(sendto, message)) {
                                              Serial.println(F("sent"));
        
                                            }
                                            else {
                                              Serial.println(F("Sent anyway!"));
                                            }
                                            textingsignal = 0;
                                             delay(100);
        
      }
      //                      //if temp sensor 2 is low send sms
      if (i == 1) {
        Serial.println("WARNING TEMP SENSOR 2 LOW");
        //                                   // send an SMS!
        char sendto[12], message[141];
                                            flushSerial();
                                            
                                        //    
                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
                                                        message[0] = 'W';
                                                        message[1] = 'A';
                                                        message[2] = 'R';
                                                        message[3] = 'N';
                                                        message[4] = 'I';
                                                        message[5] = 'N';
                                                        message[6] = 'G';
                                                        message[7] = ' ';
                                                        message[8] = 'G';
                                                        message[9] = 'H';
                                                        message[10] = '2';
                                                        message[11] = ' ';
                                                        message[12] = 'T';
                                                        message[13] = 'E';
                                                        message[14] = 'M';
                                                        message[15] = 'P';
                                                        message[16] = ' ';
                                                        message[17] = 'L';
                                                        message[18] = 'O';
                                                        message[19] = 'W';
                                                        message[20] = '\0';
                                        //    
                                           
                                            if (fona.sendSMS(sendto, message)) {
                                              Serial.println(F("sent"));
        
                                            }
                                            else {
                                              Serial.println(F("Sent anyway!"));
                                            }
                                            textingsignal = 0;
                                             delay(100);
        
      }
      //                      //if temp sensor 3 is low send sms
                            if(i==2){
                              Serial.println("WARNING TEMP SENSOR 3 LOW");
      ////                                   // send an SMS!
                                          char sendto[12], message[141];
                                          flushSerial();
                                          
                                       //   
                                          sendto[0]='1';
                                          sendto[1]='3';
                                          sendto[2]='1';
                                          sendto[3]='0';
                                          sendto[4]='9';
                                          sendto[5]='9';
                                          sendto[6]='4';
                                          sendto[7]='7';
                                          sendto[8]='1';
                                          sendto[9]='6';
                                          sendto[10]='9';
                                          sendto[11]='\0';
      
                                                        message[0] = 'W';
                                                        message[1] = 'A';
                                                        message[2] = 'R';
                                                        message[3] = 'N';
                                                        message[4] = 'I';
                                                        message[5] = 'N';
                                                        message[6] = 'G';
                                                        message[7] = ' ';
                                                        message[8] = 'G';
                                                        message[9] = 'H';
                                                        message[10] = '3';
                                                        message[11] = ' ';
                                                        message[12] = 'T';
                                                        message[13] = 'E';
                                                        message[14] = 'M';
                                                        message[15] = 'P';
                                                        message[16] = ' ';
                                                        message[17] = 'L';
                                                        message[18] = 'O';
                                                        message[19] = 'W';
                                                        message[20] = '\0';
                                       //   
                                          
                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
      
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
                                          textingsignal = 0;
                                           delay(100);
      
                            }
      //                      //if temp sensor 4 is low send sms
      if (i == 3) {
                                Serial.println("WARNING TEMP SENSOR 4 LOW");
        //                                   // send an SMS!
        char sendto[12], message[141];
                                            flushSerial();
                                           
                                        //    
                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
                                                        message[0] = 'W';
                                                        message[1] = 'A';
                                                        message[2] = 'R';
                                                        message[3] = 'N';
                                                        message[4] = 'I';
                                                        message[5] = 'N';
                                                        message[6] = 'G';
                                                        message[7] = ' ';
                                                        message[8] = 'G';
                                                        message[9] = 'H';
                                                        message[10] = '4';
                                                        message[11] = ' ';
                                                        message[12] = 'T';
                                                        message[13] = 'E';
                                                        message[14] = 'M';
                                                        message[15] = 'P';
                                                        message[16] = ' ';
                                                        message[17] = 'L';
                                                        message[18] = 'O';
                                                        message[19] = 'W';
                                                        message[20] = '\0';
                                      //      
                                            
                                            if (fona.sendSMS(sendto, message)) {
                                              Serial.println(F("sent"));
        
                                            }
                                            else {
                                              Serial.println(F("Sent anyway!"));
                                            }
                                            textingsignal = 0;
                                             delay(100);
        
      }
      //                      //if temp sensor 5 is low send sms
                            if(i==4){
                              Serial.println("WARNING TEMP SENSOR 5 LOW");
      ////                                   // send an SMS!
                                          char sendto[12], message[141];
                                          flushSerial();
                                          
                                      //    
                                          sendto[0]='1';
                                          sendto[1]='3';
                                          sendto[2]='1';
                                          sendto[3]='0';
                                          sendto[4]='9';
                                          sendto[5]='9';
                                          sendto[6]='4';
                                          sendto[7]='7';
                                          sendto[8]='1';
                                          sendto[9]='6';
                                          sendto[10]='9';
                                          sendto[11]='\0';
      
                                            message[0] = 'W';
                                            message[1] = 'A';
                                            message[2] = 'R';
                                            message[3] = 'N';
                                            message[4] = 'I';
                                            message[5] = 'N';
                                            message[6] = 'G';
                                            message[7] = ' ';
                                            message[8] = 'H';
                                            message[9] = 'H';
                                            message[10] = ' ';
                                            message[11] = 'T';
                                            message[12] = 'E';
                                            message[13] = 'M';
                                            message[14] = 'P';
                                            message[15] = ' ';
                                            message[16] = 'H';
                                            message[17] = 'I';
                                            message[18] = 'G';
                                            message[19] = 'H';
                                            message[20] = '\0';
                                     //     
                                          
                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
      
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
                                          textingsignal = 0;
                                           delay(100);
      
                            }
      ////                      //if temp sensor 6 is low send sms
                            if(i==5){
                              Serial.println("WARNING TEMP SENSOR 6 LOW");
      ////                                   // send an SMS!
                                          char sendto[12], message[141];
                                          flushSerial();
                                          
                                      //    
                                          sendto[0]='1';
                                          sendto[1]='3';
                                          sendto[2]='1';
                                          sendto[3]='0';
                                          sendto[4]='9';
                                          sendto[5]='9';
                                          sendto[6]='4';
                                          sendto[7]='7';
                                          sendto[8]='1';
                                          sendto[9]='6';
                                          sendto[10]='9';
                                          sendto[11]='\0';
      
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'I';
                                                      message[9] = 'S';
                                                      message[10] = 'O';
                                                      message[11] = '1';
                                                      message[12] = ' ';
                                                      message[13] = 'T';
                                                      message[14] = 'E';
                                                      message[15] = 'M';
                                                      message[16] = 'P';
                                                      message[17] = ' ';
                                                      message[18] = 'L';
                                                      message[19] = 'O';
                                                      message[20] = 'W';
                                                      message[21] = '\0';
                                      //    
                                          
                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
      
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
                                          textingsignal = 0;
                                           delay(100);
      
                            }
      ////                      //if temp sensor 7 is low send sms
                            if(i==6){
//                              Serial.println("WARNING TEMP SENSOR 7 LOW");
//      ////                                   // send an SMS!
//                                          char sendto[12], message[141];
//                                          flushSerial();
//                                         
//                                      //    
//                                          sendto[0]='1';
//                                          sendto[1]='3';
//                                          sendto[2]='1';
//                                          sendto[3]='0';
//                                          sendto[4]='9';
//                                          sendto[5]='9';
//                                          sendto[6]='4';
//                                          sendto[7]='7';
//                                          sendto[8]='1';
//                                          sendto[9]='6';
//                                          sendto[10]='9';
//                                          sendto[11]='\0';
//      
//                                                      message[0] = 'W';
//                                                      message[1] = 'A';
//                                                      message[2] = 'R';
//                                                      message[3] = 'N';
//                                                      message[4] = 'I';
//                                                      message[5] = 'N';
//                                                      message[6] = 'G';
//                                                      message[7] = ' ';
//                                                      message[8] = 'I';
//                                                      message[9] = 'S';
//                                                      message[10] = 'O';
//                                                      message[11] = '2';
//                                                      message[12] = ' ';
//                                                      message[13] = 'T';
//                                                      message[14] = 'E';
//                                                      message[15] = 'M';
//                                                      message[16] = 'P';
//                                                      message[17] = ' ';
//                                                      message[18] = 'L';
//                                                      message[19] = 'O';
//                                                      message[20] = 'W';
//                                                      message[21] = '\0';
//                                    //      
//                                          
//                                          if (fona.sendSMS(sendto, message)) {
//                                            Serial.println(F("sent"));
//      
//                                          }
//                                          else {
//                                            Serial.println(F("Sent anyway!"));
//                                          }
//                                          textingsignal = 0;
//                                           delay(100);
      
                            }
      ////                      //if temp sensor 8 is low send sms
                            if(i==7){
                              Serial.println("WARNING TEMP SENSOR 8 LOW");
      ////                                   // send an SMS!
                                          char sendto[12], message[141];
                                          flushSerial();
                                          
                                     //     
                                          sendto[0]='1';
                                          sendto[1]='3';
                                          sendto[2]='1';
                                          sendto[3]='0';
                                          sendto[4]='9';
                                          sendto[5]='9';
                                          sendto[6]='4';
                                          sendto[7]='7';
                                          sendto[8]='1';
                                          sendto[9]='6';
                                          sendto[10]='9';
                                          sendto[11]='\0';
      
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'I';
                                                      message[9] = 'S';
                                                      message[10] = 'O';
                                                      message[11] = '3';
                                                      message[12] = ' ';
                                                      message[13] = 'T';
                                                      message[14] = 'E';
                                                      message[15] = 'M';
                                                      message[16] = 'P';
                                                      message[17] = ' ';
                                                      message[18] = 'L';
                                                      message[19] = 'O';
                                                      message[20] = 'W';
                                                      message[21] = '\0';
                                    //      
                                          
                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
      
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
                                          textingsignal = 0;
                                           delay(100);
      
                            }
      ////                      //if temp sensor 9 is low send sms
                            if(i==8){
                              Serial.println("WARNING TEMP SENSOR 9 LOW");
      ////                                   // send an SMS!
                                          char sendto[12], message[141];
                                          flushSerial();
                                          
                                   //       
                                          sendto[0]='1';
                                          sendto[1]='3';
                                          sendto[2]='1';
                                          sendto[3]='0';
                                          sendto[4]='9';
                                          sendto[5]='9';
                                          sendto[6]='4';
                                          sendto[7]='7';
                                          sendto[8]='1';
                                          sendto[9]='6';
                                          sendto[10]='9';
                                          sendto[11]='\0';
      
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'I';
                                                      message[9] = 'S';
                                                      message[10] = 'O';
                                                      message[11] = '4';
                                                      message[12] = ' ';
                                                      message[13] = 'T';
                                                      message[14] = 'E';
                                                      message[15] = 'M';
                                                      message[16] = 'P';
                                                      message[17] = ' ';
                                                      message[18] = 'L';
                                                      message[19] = 'O';
                                                      message[20] = 'W';
                                                      message[21] = '\0';
                                   //       
                                          
                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
      
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
                                          textingsignal = 0;
                                           delay(100);
      
                            }
      ////                      //if temp sensor 10 is low send sms
                            if(i==9){
                              Serial.println("WARNING TEMP SENSOR 10 LOW");
      //                                   // send an SMS!
                                          char sendto[12], message[141];
                                          flushSerial();
                                         
                                   //       
                                          sendto[0]='1';
                                          sendto[1]='3';
                                          sendto[2]='1';
                                          sendto[3]='0';
                                          sendto[4]='9';
                                          sendto[5]='9';
                                          sendto[6]='4';
                                          sendto[7]='7';
                                          sendto[8]='1';
                                          sendto[9]='6';
                                          sendto[10]='9';
                                          sendto[11]='\0';
      
                                                      message[0] = 'W';
                                                      message[1] = 'A';
                                                      message[2] = 'R';
                                                      message[3] = 'N';
                                                      message[4] = 'I';
                                                      message[5] = 'N';
                                                      message[6] = 'G';
                                                      message[7] = ' ';
                                                      message[8] = 'L';
                                                      message[9] = 'H';
                                                      message[10] = ' ';
                                                      message[11] = 'T';
                                                      message[12] = 'E';
                                                      message[13] = 'M';
                                                      message[14] = 'P';
                                                      message[15] = ' ';
                                                      message[16] = 'L';
                                                      message[17] = 'O';
                                                      message[18] = 'W';
                                                      message[19] = '\0';
                                    //      
                                          
                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
      
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
                                          textingsignal = 0;
 delay(100);
      
                            }
 
      if(currentTime -previousTimeLow < 0){
                      previousTimeLow = 0;
                    }
    }
            }previousTimeLow =currentTime;
  }
} // end of loop
 

///////////////////////////////////////////////////////////////////////
//choose which mux to transmit data from, Ex. mux1, mux2, mux3 or mux4//
void tcaselect(uint8_t i) {
  // set the mux we want to the correct loacation// reading from mux1, disable other muxes
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
  //disable all other muxes
  Wire.beginTransmission(TCAADDR2);
  Wire.write(0); // no channel selected
  Wire.endTransmission();
  Wire.beginTransmission(TCAADDR3);
  Wire.write(0); // no channel selected
  Wire.endTransmission();
  Wire.beginTransmission(TCAADDR4);
  Wire.write(0); // no channel selected
  Wire.endTransmission();
}

//reading from mux2, disable other muxes//////////////////////////////////////////////////////////////////////
void tcaselect2(uint8_t j) {
  if (j > 7) return;
  Wire.beginTransmission(TCAADDR2);
  Wire.write(1 << j);
  Wire.endTransmission();
  //end other muxes
  Wire.beginTransmission(TCAADDR);
  Wire.write(0); // no channel selected
  Wire.endTransmission();
  Wire.beginTransmission(TCAADDR3);
  Wire.write(0); // no channel selected
  Wire.endTransmission();
  Wire.beginTransmission(TCAADDR4);
  Wire.write(0); // no channel selected
  Wire.endTransmission();
}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

void sensorwritesd(){
        //
        //Serial.println("Writing to sd card");
      myFile = SD.open("GHMON.txt", FILE_WRITE);
      Serial.println("GHMON.txt opening");
      // if the file opened okay, write to it:
      if (myFile) {
              Serial.println("Writing to SD");
              // Send Day-of-Week//////////////////
              myFile.print(rtc.getDOWStr());
              myFile.print("\t");
              // Send date/////////////////////////
              myFile.print("D-M-Y: ");
              myFile.print(rtc.getDateStr());
               // Send time/////////////////////////
              myFile.print("\t");
              myFile.print("Time: ");
              myFile.println(rtc.getTimeStr());
              myFile.println("");
               delay(100);
                    
              
                
                  myFile.print("GH1: ");
                    //sensor data for temperature
                    myFile.print(" temperature:");
                    myFile.print(temperature[0]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[0]);
                    delay(100);

                
                
                  myFile.print("GH2: ");
                  //sensor data for temperature
                    myFile.print(" temperature:");
                    myFile.print(temperature[1]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[1]);
                    delay(100);
                    
                
                
                  myFile.print("GH3: ");
                  //sensor data for temperature
                    myFile.print(" temperature:");
                    myFile.print(temperature[2]);
                     myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[2]);
                   delay(100);
                    
                
                
                  myFile.print("GH4: ");
                  //sensor data for temperature
                    myFile.print(" temperature:");
                    myFile.print(temperature[3]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[3]);
                    delay(100);
                    
                
                
                  myFile.print("LH: ");
                  //sensor data for temperature
                    myFile.print("  temperature:");
                    myFile.print(temperature[4]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[4]);
                    delay(100);
                    
                
                
                  myFile.print("ISO1: ");
                  //sensor data for temperature
                    myFile.print("temperature:");
                    myFile.print(temperature[5]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[5]);
                    delay(100);
                    
                
                
                  myFile.print("ISO2: ");
                  //sensor data for temperature
                    myFile.print("temperature:");
                    myFile.print(temperature[6]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[6]);
                    delay(100);
                    
                
                
                  myFile.print("ISO3: ");
                  //sensor data for temperature
                    myFile.print("temperature:");
                    myFile.print(temperature[7]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[7]);
                    delay(100);
                    
                
                
                  myFile.print("ISO4: ");
                  //sensor data for temperature
                    myFile.print("temperature:");
                    myFile.print(temperature[8]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[8]);
                    delay(100);
                    
                
                
                  myFile.print("HH: ");
                  //sensor data for temperature
                  myFile.print("  temperature:");
                    myFile.print(temperature[9]);
                    myFile.print(" ");
                   // sensor data for humidity
                    myFile.print("humidity:");
                    myFile.println(humidity[9]);
                    
                    
                    
                        
              
              delay(100);
              myFile.println("_______________________________end____________________________________ ");
              myFile.println(" ");
              Serial.println("GHMON.txt closing");
        // close the file:
          myFile.close();
            }

            else {
              // if the file didn't open, print an error:
              Serial.println("error opening GHMON.txt");
                  }
}



void voltageMonitor() {

}
//Interrupt if power goes low
void POWER_LOW(){
  //SEND MESSAGE
  powerOut = true;
  Serial.println("POWER OUTAGE");
        char sendto[12], message[141];
                                            flushSerial();
                                            //Serial.print(F("Send to #"));
                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
        
                                            message[0] = 'W';
                                            message[1] = 'A';
                                            message[2] = 'R';
                                            message[3] = 'N';
                                            message[4] = 'I';
                                            message[5] = 'N';
                                            message[6] = 'G';
                                            message[7] = ' ';
                                            message[8] = 'P';
                                            message[9] = 'O';
                                            message[10] = 'W';
                                            message[11] = 'E';
                                            message[12] = 'R';
                                            message[13] = ' ';
                                            message[14] = 'O';
                                            message[15] = 'U';
                                            message[16] = 'T';
                                            message[17] = 'A';
                                            message[18] = 'G';
                                            message[19] = 'E';
                                            message[20] = '!';
                                            message[21] = '\0';

                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
        
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
        textingsignal = 0;
        powerCount = 0;
        delay(100);  
  }
//Interrupt if power goes back high
void POWER_HIGH(){
  powerOut = false;
   //SEND MESSAGE
  Serial.println("POWER OUTAGE");
        char sendto[12], message[141];
                                            flushSerial();
                                            //Serial.print(F("Send to #"));
                                            sendto[0]='1';
                                            sendto[1]='3';
                                            sendto[2]='1';
                                            sendto[3]='0';
                                            sendto[4]='9';
                                            sendto[5]='9';
                                            sendto[6]='4';
                                            sendto[7]='7';
                                            sendto[8]='1';
                                            sendto[9]='6';
                                            sendto[10]='9';
                                            sendto[11]='\0';
        
        
                                            message[0] = 'P';
                                            message[1] = 'O';
                                            message[2] = 'W';
                                            message[3] = 'E';
                                            message[4] = 'R';
                                            message[5] = ' ';
                                            message[6] = 'N';
                                            message[7] = 'O';
                                            message[8] = 'R';
                                            message[9] = 'M';
                                            message[10] = 'A';
                                            message[11] = 'L';
                                            message[12] = 'I';
                                            message[13] = 'Z';
                                            message[14] = 'E';
                                            message[15] = 'D';
                                            message[16] = '\0';

                                          if (fona.sendSMS(sendto, message)) {
                                            Serial.println(F("sent"));
        
                                          }
                                          else {
                                            Serial.println(F("Sent anyway!"));
                                          }
        textingsignal = 0;
        delay(100);  
        powerCount = 0;  
  }
