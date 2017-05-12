

#include <TinyGPS++.h>
#include <SoftwareSerial.h>


//GSM Module initializations
int8_t answer;
int onModulePin= 10;
char aux_string[30];
char phone_number[]="0241274040";

//GPS Module initializations
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
float latitude;
float longitude;
float timeSec;



//Function declarations
void power_on();
void getGPSData();
void sendSMS(float a, float b, float c);
float getLatitude();
float getLongitude();

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
    latitude = gps.location.lat();
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
     longitude = gps.location.lat();
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

  sendSMS(latitude, longitude, timeSec);
}



/*---------------------------------------------------
  GSM MODULE FUNCTIONS
----------------------------------------------------*/


//The Function Which Sends Text Messages. Takes in 3 parameters..
// 1. First parameter specifies what kind of message it should send
//2. Second parameter is the Fuel level measured
//3. Third parameter is a pointer(reference to longitude and latitude)

void sendSMS(float a, float b, float c){
   //sets the PIN code

  // float *gpsInfo;
 //  gpsInfo
    sendATcommand("AT+CPIN=****", "OK", 2000);
    delay(3000);
    Serial.println("Connecting to the network...");
   
    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );
    Serial.print("Setting SMS mode...");
    delay(1000);
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");

    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    {   
        Serial.print("Longitude "); Serial.print(a); Serial.print("Latitude; ");  Serial.print( b); Serial.print("TIme ");  Serial.print( c); 
        //YOUR HTTPPOST COMMAND GOES HERE
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.print("Sent ");  
             delay(1000);  
        }
        else
        {
            Serial.print("error, you are out of credit ");
             delay(1000);
        }
    }
    else
    {
        Serial.print("error setting sms mode ");
             delay(2000);
        Serial.println(answer, DEC);
    }

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


