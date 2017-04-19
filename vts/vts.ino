#include <TinyGPS++.h>
#include <SoftwareSerial.h>


// The serial connection to the GPS device
SoftwareSerial ss(4, 3);
SoftwareSerial gprsSerial(7, 8);


// The TinyGPS++ object
TinyGPSPlus gps;

int timeElapsed;
int getCoordinates();

void setup()
{
  Serial.begin(9600);
   ss.begin(9600);
  gprsSerial.begin(9600);
 
  

  Serial.println("Config SIM900...");
  delay(2000);
  Serial.println("Done!...");
  gprsSerial.flush();
  Serial.flush();

  // attach or detach from GPRS service 
  gprsSerial.println("AT+CGATT?");
  delay(100);
  toSerial() ;


  // bearer settings
  gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(2000);
  toSerial();

  // bearer settings
  gprsSerial.println("AT+SAPBR=3,1,\"internet\",\"vodafone\"");
  delay(2000);
  toSerial();

  // bearer settings
  gprsSerial.println("AT+SAPBR=1,1");
  delay(2000);
  toSerial();
}


void loop()
{
  

  //variables to hold lat and long
    int latvalue = getCoordinates();//"newlat1";
    Serial.println(latvalue);
    /*
    //int longvalue = getCoordinates();//"newlong1";

  
   // initialize http service
   gprsSerial.println("AT+HTTPINIT");
   delay(2000); 
   toSerial();

   // set http param value
//   gprsSerial.println("AT+HTTPPARA=\"URL\",\"http://www.kenkey.xyz/api/location/?latitude=7.673778&longitude=-1.565546\""); 
//   delay(2000);
//   toSerial();

  gprsSerial.print("AT+HTTPPARA=\"URL\",\"http://www.kenkey.xyz/api/location/?"); //latitude=6.673778&longitude=-1.565546
  gprsSerial.print("latitude=");  // This is the value name
  gprsSerial.print(latvalue); // value of...
  gprsSerial.print("&longitude=");  // This is the value name
  gprsSerial.print(longvalue); // value of...
  gprsSerial.println("\"");
  delay(2000);
   toSerial();

   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   gprsSerial.println("AT+HTTPACTION=1");
   delay(6000);
   toSerial();

   // read server response
   gprsSerial.println("AT+HTTPREAD"); 
   delay(1000);
   toSerial();

   gprsSerial.println("");
   gprsSerial.println("AT+HTTPTERM");
   toSerial();
   delay(300);

   gprsSerial.println("");
   delay(10000);
   */
}

void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}


int getCoordinates(){
    // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
     
  Serial.print(F("currenttime: "));
  if (gps.time.isValid())
  {
    if (gps.time.second() < 10) Serial.print(F("0"));
    timeElapsed = gps.time.second(); 
    Serial.print(timeElapsed);

  }
  else
  {
    Serial.print(F("INVALID"));
  }


  /*PUT CODE TO GET LONGITUDE AND LATITUDE HERE
  
  return(lon, lati)
  
  */


  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
      return(timeElapsed);
}

