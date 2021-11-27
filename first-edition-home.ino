/*
  Rui Santos
  Complete project details at Complete project details at https://ewink.ir/wiki
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>


const char* ssid = "saye24";
const char* password = "saye1234";
//Your Domain name with URL path or IP address with path
const String serverName = "http://130.185.76.58:9292/arduino_recive.php";
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
String sensorReadings;
float sensorReadingsArr[3];
const int PUMP = 2;
const int LAMP = 4;
const int DARB = 5;
const int PIR = 13;

int pirStat = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PUMP, OUTPUT);  
  pinMode(LAMP, OUTPUT);  
  pinMode(DARB, OUTPUT);  

  pinMode(PIR , INPUT);
  
  digitalWrite(PUMP, LOW);
  digitalWrite(LAMP, LOW);
  digitalWrite(DARB, LOW);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}


void loop() {
   checkPIR();
   
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      sensorReadings = httpGETRequest(serverName);
      Serial.println(sensorReadings);
      JSONVar myObject = JSON.parse(sensorReadings);
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      Serial.println(myObject);
      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();

    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void checkPIR()
{
    int pirLocal = pirStat;
    pirStat = digitalRead(PIR);

    if(pirStat != pirLocal)
      Serial.println("state="+pirStat);

}

String httpGETRequest(const String serverName) {
  HTTPClient http;
  // Your IP address with path or Domain name with URL path
  String req = serverName+"?pir_anbar="+pirStat;
  //char* buf;
  //req.toCharArray(buf,50);
  Serial.println(req);
  http.begin(req);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  //Serial.println(" res" + http.begin(req));

  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    JSONVar myObject = JSON.parse(payload);

    int pump = (int)myObject["pump"];
    int lamp = (int)myObject["lamp"];
    int darb = (int)myObject["darb"];

    if (pump == 1)
    {
      Serial.println("Pump ON");
      digitalWrite(PUMP, LOW);
    } else
    {
      Serial.println("PUMP OFF");
      digitalWrite(PUMP, HIGH);
    }

    if (lamp == 1)
    {
      Serial.println("LAMP ON");
      digitalWrite(LAMP, LOW);
    } else
    {
      Serial.println("LAMP OFF");
      digitalWrite(LAMP, HIGH);
    }

    if (darb == 1 || pirStat == 1)
    {
      Serial.println("DARB ON");
      digitalWrite(DARB, LOW);
    } else
    {
      Serial.println("DARB OFF");
      digitalWrite(DARB, HIGH);
    }

  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}
