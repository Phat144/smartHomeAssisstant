#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h> 
#include <ESP8266mDNS.h>
#include "DHT.h"

//define temperature sensor pin and type
#define DHTPIN 5
#define DHTTYPE DHT11

MDNSResponder mdns;

//wifi connection 
const char* ssid = "Peter";
const char* password = "single14493";

ESP8266WebServer server(80);  //define server on port 80
String webPage = "";

//============Temperature sensor defined
DHT dht(DHTPIN, DHTTYPE);

//============flame sensor define
// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum


void setup(void) {
  // put your setup code here, to run once:
  dht.begin();  //start temp sensor
  delay(1000);
  Serial.begin(115200);

  //Wifi output IP address if connected
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started. ");
  }
//==========================================

tempReader(); //start temperature online reader
flameReader(); //start flame detector

  

  server.on("/", [](){
    server.send(200, "text/html", "Welcome to the weather station!");
  });
  server.begin();
  Serial.println("HTTPS Server started. ");
}

void loop(void) {
  // put your main code here, to run repeatedly:
  
  server.handleClient();
}

//Get temperature data and send to webserver
void tempReader(){
    server.on("/readTemp", [](){
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    char resulttemp[14] = "";
    char resulthum[14] = "";
    dtostrf(temp, 0, 1, resulttemp);
    dtostrf(hum, 0, 1, resulthum);
    webPage = "Temperature: ";
    webPage += resulttemp;
    webPage += "  Humidity: ";
    webPage += resulthum;
    server.send(200, "text/html", webPage);
    delay(1);
  });
}

void flameReader(){
  server.on("/readFlame", [](){
    // read the sensor on analog A0:
    int sensorReading = analogRead(A0);
    // map the sensor range (four options):
    // ex: 'long int map(long int, long int, long int, long int, long int)'
    int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
    
    // range value:
    switch (range) {
    case 2:    // A fire closer than 1.5 feet away.
      webPage = "Fire Detected";
      Serial.println("Close Fire");
      break;
    case 1:    // A fire between 1-3 feet away.
      webPage = "Fire Detected";
      Serial.println("Distant Fire");
      break;
    case 0:    // No fire detected.
      webPage = "No Fire";
      Serial.println("No Fire");
      break;
    }
      server.send(200, "text/html", webPage);
      delay(1);
  });
}
