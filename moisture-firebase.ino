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
#define WIFI_SSID "why"
#define WIFI_PASSWORD "wahyu123"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBnkCUlVyOtaz369WUwMbXQK700HMCtw7U"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://moisture-c02cc-default-rtdb.firebaseio.com" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

const int MOISTURE_PIN = A0;
const int RELAY_PIN = D3;

const int MOISTURE_THRESHOLD = 500;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;

void setup(){
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  digitalWrite(RELAY_PIN, LOW);
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
}

void loop(){
 delay(1000);

  int moistureLevel = analogRead(MOISTURE_PIN);

  Serial.print("kelembaban tanah sebesar: ");
  Serial.println(moistureLevel);
  
  if (Firebase.ready() && signupOK ) {
    

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", moistureLevel)){
//      Serial.println("PASSED");
       Serial.print("Kelembaban Tanah: ");
       Serial.println(moistureLevel);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  Serial.println("______________________________");
}