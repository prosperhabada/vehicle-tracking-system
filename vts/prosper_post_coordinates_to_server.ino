

#include <TinyGPS++.h>
#include <SoftwareSerial.h>


//GSM Module initializations
int8_t answer;
int onModulePin= 10;
char aux_string[30];
char phone_number[]="0501349498";

//GPS Module initializations
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
double latitude;
double longitude;
double timeSec;



//Function declarations
void power_on();
void getGPSData();
//void sendSMS(float a, float b, float c);
void sendPOST(double latitude, double longitude);


TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial gpsModule(RXPin, TXPin);

void setup(){
  gpsModule.begin(GPSBaud);
  Serial.begin(57600);
  
  pinMode(onModulePin, OUTPUT);


   //SETUP GSM MODULE
  power_on();
  delay(2000);

   //SETUP GPS MODULE
 
   delay(1000);

}




/*---------------------------------------------------
LOOP FUNCTIONS
----------------------------------------------------*/
void loop(){
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsModule.available() > 0)
    if (gps.encode(gpsModule.read()))
      getGPSData();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  //  while(true);
  }

}




/*---------------------------------------------------
  GPS MODULE FUNCTIONS
----------------------------------------------------*/

//SET THE GPS MODULE UP
void getGPSData()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    latitude = gps.location.lat(), 6;
    
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    longitude = gps.location.lng(), 6;
    
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
      /*if (gps.time.hour() < 10) Serial.print(F("0"));
  Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":")); */
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));

    timeSec = gps.time.second();
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();

//  sendSMS(latitude, longitude, timeSec);
  sendPOST(latitude, longitude);
}



//The Function Which Sends gps coordinates to webserver. Takes in 2 parameters..
// 1. First parameter is the latitude
//2. Second parameter is the longitude





void sendPOST(double latitude, double longitude){


    // attach or detach from GPRS service 
    sendATcommand("AT+CGATT?", "OK", 2000);
    delay(100);
    toSerial();
  
  
    // bearer settings
    sendATcommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", 2000);
    delay(2000);
    toSerial();
  
    // bearer settings
    sendATcommand("AT+SAPBR=3,1,\"internet\",\"vodafone\"", "OK", 2000);
    delay(2000);
    toSerial();
  
    // bearer settings
    sendATcommand("AT+SAPBR=1,1", "OK", 2000 );
    delay(2000);
    toSerial();

    // initialize http service
    sendATcommand("AT+HTTPINIT" , "OK", 2000);
    delay(2000); 
    toSerial();

     // set http param value
//    sendATcommand("AT+HTTPPARA=\"URL\",\"http://www.kenkey.xyz/api/location/?latitude=7.673778&longitude=-1.565546\"" , "OK", 2000);
//    delay(2000);
//    toSerial();

    Serial.print("AT+HTTPPARA=\"URL\",\"http://www.kenkey.xyz/api/location/?"); //latitude=6.673778&longitude=-1.565546
    Serial.print("latitude=");  // This is the value name
    Serial.print(latitude, 6); // value of...
    Serial.print("&longitude=");  // This is the value name
    Serial.print(longitude, 6); // value of...
    sendATcommand("\"", "OK", 2000);
    delay(2000);
     toSerial();

   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   sendATcommand("AT+HTTPACTION=1" , "OK", 2000);
   delay(4000);
   toSerial();
   

   // read server response
   sendATcommand("AT+HTTPREAD", "OK", 2000); 
   delay(1000);
   toSerial();

   Serial.println("");
   sendATcommand("AT+HTTPTERM", "OK", 2000);
   toSerial();
   delay(300);

   Serial.println("");
   delay(5000);


}


//Function to turn on the GSM module
void power_on(){

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
    
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
    
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string
    
    delay(100);
    
    while( Serial.available() > 0) Serial.read();    // Clean the input buffer
    
    Serial.println(ATcommand);    // Send the AT command 


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(Serial.available() != 0){    
            response[x] = Serial.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
                
                    
            }
        }
    // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}



void toSerial()
{
  while(Serial.available()!=0)
  {
    Serial.write(Serial.read());
  }
}
