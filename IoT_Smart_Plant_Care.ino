#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Insert Firebase project API Key
#define API_KEY ""
// Insert RTDB URLefine the RTDB URL */
//https://vehiclecrashgps-default-rtdb.firebaseio.com/
//https://soilmoistureiot-default-rtdb.firebaseio.com/
//https://iotdata-79450-default-rtdb.firebaseio.com/
#define DATABASE_URL "iotdata-79450-default-rtdb.firebaseio.com"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 2//d4

const int soilsen =A0;
const int waterpump =D6;
const int fan =D7;
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;




void setup(){
  Serial.begin(115200);
  pinMode(soilsen, INPUT);
  pinMode(waterpump, OUTPUT);
  pinMode(fan, OUTPUT);
  digitalWrite(fan,LOW);
  digitalWrite(waterpump,LOW);
  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("Conecting.....");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  lcd.clear();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Green House");
  lcd.setCursor(0, 1);
  lcd.print("Automation System");
  delay(3000);
  lcd.clear();
}

void loop(){
  DHT.read(DHT11_PIN);
  Serial.println("temp:");
  Serial.print(DHT.temperature);
  int temp=DHT.temperature;
  Serial.print("  humi:");
  Serial.println(DHT.humidity);


  if (Firebase.RTDB.setInt(&fbdo, "Temp", temp)){
    }
if(temp>35)
{
  Serial.println("Turn on Fan");
 digitalWrite(fan,HIGH);
   lcd.setCursor(0, 0);
  lcd.print("Temp:");
    lcd.setCursor(6, 0);
  lcd.print(temp);
      lcd.setCursor(9, 0);
  lcd.print("oC");

      lcd.setCursor(0, 1);
  lcd.print("Fan sts:");

   lcd.setCursor(8, 1);
  lcd.print("Fan ON");
    if (Firebase.RTDB.setString(&fbdo, "Fan","ON")){
    }  


}
else
{
  Serial.println("Turn off Fan");
 digitalWrite(fan,LOW);
    lcd.setCursor(0, 0);
  lcd.print("Temp:");
     lcd.setCursor(6, 0);
  lcd.print(temp);
        lcd.setCursor(9, 0);
  lcd.print("oC");


  lcd.setCursor(0, 1);
  lcd.print("Fan sts:");

   lcd.setCursor(8, 1);
  lcd.print("Fan OFF");
      if (Firebase.RTDB.setString(&fbdo, "Fan","OFF")){
    }  


}
 delay(3000);
  lcd.clear();



int soilsensts=analogRead(soilsen);
Serial.println(soilsensts);
if(soilsensts < 550)
{
  Serial.println("Enough moisture");
 digitalWrite(waterpump,LOW);
   lcd.setCursor(0, 0);
  lcd.print("Moist:");

   lcd.setCursor(6, 0);
  lcd.print("Sufficient");
  lcd.setCursor(0, 1);
  lcd.print("Pump:");

   lcd.setCursor(6, 1);
  lcd.print("Pump Off");
  if (Firebase.RTDB.setString(&fbdo, "Moist:","Sufficient")){
    }  
  if (Firebase.RTDB.setString(&fbdo, "Pump","OFF")){
    }
}
else
{
Serial.println("need water");

   lcd.setCursor(0, 0);
  lcd.print("Moist:");
   lcd.setCursor(6, 0);
  lcd.print("Need water");
    lcd.setCursor(0, 1);
  lcd.print("Pump:");
   lcd.setCursor(6, 1);
  lcd.print("Pump ON");

  digitalWrite(waterpump,HIGH);
  delay(2000);
    if (Firebase.RTDB.setString(&fbdo, "Moist:","Need_Water")){
    }  
    if (Firebase.RTDB.setString(&fbdo, "Pump","ON")){
    }  
}


  delay(3000);
  lcd.clear();
}
