#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#define DHTPIN 14 
#define DHTTYPE DHT22

const int oneWireBus = 12;     


OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
HTU21D myHumidity;
DHT dht(DHTPIN, DHTTYPE);


char ssid[]= "WIFI-CIE-UNAM-101";
const char* password = "";

#define TOKEN "zkzJYuU3Nuqryyx4eySS" //Access token of device Display
char ThingsboardHost[] = "iot.ier.unam.mx";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("connected to");
  Serial.println(ssid);
  client.setServer( ThingsboardHost, 1883 );
  
  sensors.begin();
  myHumidity.begin();
  dht.begin();
}

void loop()
{
if ( !client.connected() ) 
{
    reconnect();
}
temperatura18();
temperaturayhumedad21();
temperaturayhumedad22();

  delay(600000);
}

void temperatura18()
{
  sensors.requestTemperatures();
  
  float Temp18 = sensors.getTempCByIndex(0);

  // Prepare a JSON payload string
  String payload = "{";
 payload += "\"Temperatura18\":";payload += Temp18; 
 payload += "}";

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish( "v1/devices/me/telemetry",attributes);
  Serial.println( attributes );
   
}

void temperaturayhumedad21()
{
  float Hum21 = myHumidity.readHumidity();
  float Temp21 = myHumidity.readTemperature();

  // Prepare a JSON payload string
  String payload = "{";
 payload += "\"Humedad21\":";payload += Hum21; payload += ",";
 payload += "\"Temperatura21\":";payload += Temp21;
 payload += "}";

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish( "v1/devices/me/telemetry",attributes);
  Serial.println( attributes );
   
}

void temperaturayhumedad22()
{
  
  float Hum22 = dht.readHumidity();
  float Temp22 = dht.readTemperature(); 

  // Prepare a JSON payload string
  String payload = "{";
 payload += "\"Humedad22\":";payload += Hum22; payload += ",";
 payload += "\"Temperatura22\":";payload += Temp22;
 payload += "}";

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish( "v1/devices/me/telemetry",attributes);
  Serial.println( attributes );
   
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Esp8266", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.println( " : retrying in 5 seconds]" );
      delay( 500 );
    }
  }
}
