#include "DHT.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <TroykaCurrent.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define WIFI_AP "PACCBET"
#define WIFI_PASSWORD "020FPass"

#define TOKEN "AJyvkQ9jU8KvsU5rZ4sd"

// DHT
#define DHTPIN 5
#define DHTTYPE DHT22
ACS712 sensorCurrent(2);
String payload;
float latitude , longitude, current;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str,str, datetime, cur_str;
int pm;
static const int RXPin = 13, TXPin = 15;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
char thingsboardServer[] = "demo.thingsboard.io";

WiFiClient wifiClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  delay(10);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
}

void loop()
{
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
     displayInfo();display_current();
    lastSend = millis();
  }

  client.loop();
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  String temperature = String(t);
  String humidity = String(h);


  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
 payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );

}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}




void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
     latitude = gps.location.lat();latitude=55.66990489;
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();longitude=37.48075962;
        lng_str = String(longitude , 6);
       // if (lat_str==""){lat_str="0";}
         //if (lng_str==""){lng_str="0";}
          // if (lat_str==NULL){lat_str="0";}
         //if (lng_str==NULL){lng_str="0";}
        // lat_str="0";lng_str="0";
        
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
     date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
          date_str = '0';
        date_str += String(date);

        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month);

        date_str += " / ";

        if (year < 10)
          date_str += '0';
        date_str += String(year);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());

time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

       // minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 3) ;
        if (hour > 23)
          hour = hour - 24;

        if (hour >= 12)
          pm = 1;
        else
          pm = 0;

        hour = hour % 12;

        if (hour < 10)
          time_str = '0';
        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)
          time_str += '0';
        time_str += String(minute);

        time_str += " : ";

        if (second < 10)
          time_str += '0';
        time_str += String(second);

        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";



    
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
Serial.println("Lat");
Serial.println(lat_str);
Serial.println("Long");
Serial.println(lng_str);
Serial.println("Date");
Serial.println(date_str);
Serial.println("Time");
Serial.println(time_str);
//char c1[]=;
 latitude = gps.location.lat();latitude=55.66990489;
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();longitude=37.48075962;
        lng_str = String(longitude , 6);
    //client.publish("lat", (char*) lat_str.c_str());
      //client.publish("long", (char*) lng_str.c_str());
      //client.publish("time", (char*) time_str.c_str());
      //client.publish("date", (char*) date_str.c_str());
 //client.loop(); 
payload="{";
  payload += "\"latitude\":"; payload += lat_str; payload += ",";
  payload += "\"longitude\":"; payload += lng_str;
  payload += "}";
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}



void display_current()
{
 Serial.print("Current is ");
 Serial.print(sensorCurrent.readCurrentAC());
 Serial.println(" A");  

    current = sensorCurrent.readCurrentAC();
        cur_str = String(current , 6);

    // client.publish("cur", (char*) cur_str.c_str());
payload="{";
      payload += "\"current\":"; payload += cur_str; payload += "}";

char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}
