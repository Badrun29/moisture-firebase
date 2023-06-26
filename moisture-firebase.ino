#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"

#include "addons/RTDBHelper.h"

// ini untuk konfugurasi wifi
#define WIFI_SSID "iPhone kentang"
#define WIFI_PASSWORD "44444444"

//API KEY FIREBASE UNTUK KIRIM DATA KE FIREBASE
#define API_KEY "AIzaSyBnkCUlVyOtaz369WUwMbXQK700HMCtw7U"
#define DATABASE_URL "https://moisture-c02cc-default-rtdb.firebaseio.com"


FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// DEFINE VARIABLE PIN SENSOR DAN RELAY
const int MOISTURE_PIN = A0;
int RELAY_PIN = D3;
int RELAY_PIN1 = D3;
const int MOISTURE_THRESHOLD = 600;


bool signupOK = false;
String sValue, sValue2;
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(115200 );
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  digitalWrite(RELAY_PIN, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  config.api_key = API_KEY;


  config.database_url = DATABASE_URL;


  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }


  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  delay(1000);

  // VARIABEL UNTUK MEMBACA NILAI SENSOR
  int moistureLevel = analogRead(MOISTURE_PIN);


  Serial.print("kelembaban tanah sebesar: ");
  Serial.println(moistureLevel);

  if (Firebase.ready() && signupOK ) {


    // UNTUK MENGIRIMKAN DATA SENSOR KE FIREBASE
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", moistureLevel)) {
      Serial.println("PASSED");
      Serial.print("Kelembaban Tanah: ");
      Serial.println(moistureLevel);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.getString(&fbdo, "/DHT/relay")) {
      if (fbdo.dataType() == "string") {
        sValue = fbdo.stringData();
        int a = sValue.toInt();
        Serial.println(a);
        if (a == 1 && moistureLevel > MOISTURE_THRESHOLD)  {
          digitalWrite(RELAY_PIN, HIGH);
        } else {
          digitalWrite(RELAY_PIN, LOW);
        }
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }



  }


}
