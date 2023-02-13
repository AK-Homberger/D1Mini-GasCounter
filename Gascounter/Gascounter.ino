/*
  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Gas counter with WLAN
// Version 1.2, 13.02.2022, AK-Homberger

#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

//#include "index_html_de.h"    // Web site information for Gauge / Buttons in German
#include "index_html_en.h"      // Web site information for Gauge / Buttons in English

#define USE_MQTT          true  // Set to false if no MQTT to be used

#define TIME_24_HOUR      true
#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time

// Wifi
const char *ssid = "ssid";           // Set WLAN name
const char *password = "password";   // Set password

// MQTT
#if USE_MQTT == true
WiFiClient Mqtt;
PubSubClient client(Mqtt);
const char* mqtt_server = "192.168.0.71";
#endif

// Web Server
ESP8266WebServer server(80);         // Web Server at port 80

// Gas Counter
#define Gas_Counter_Pin D5           // Counter impulse is measured as interrupt on pin D5
#define GasDelta 0.01                // 0.01 m^3 per counter

double GasCounter = 0;               // Counter for gas events
double DeltaCounter = 0;
double DayCounter = 0;
double DayBeforeCounter = 0;
double DayEndCounter = 0;
double m3h = 0;
double m3h_akt = 0;
unsigned long reset_time = 0;

double MinuteValue[61];             // Ring buffer for minute values
unsigned MinutePtr = 0;             // Ring buffer pointer
bool FullHour = false;              // Full hour data stored

volatile unsigned long StartValue = 0;            // First interrupt value
volatile unsigned long PeriodCount  = 0;          // period time in ms
volatile unsigned long Last_int_time = 0;         // Stores last Interrupt time


//*****************************************************************************
// Gas Event Interrupt
// Enters on falling edge

void IRAM_ATTR handleInterrupt() {
  noInterrupts();
  if (millis() - Last_int_time > 100) { // Debouncing 100 ms for reed contact
    
    DeltaCounter += GasDelta;           // Gas COUNT event: Count up
    DayCounter += GasDelta;
    GasCounter += GasDelta;             
    
    unsigned long TempVal = millis();   // Time period for average calculation
    PeriodCount = TempVal - StartValue;
    StartValue = TempVal;
    Last_int_time = millis();    
  }  
  interrupts();
}


//*****************************************************************************
void setup() {
  int wifi_retry = 0;
  struct tm local;

  // Init gas gount measure with interrupt
  pinMode(Gas_Counter_Pin, INPUT_PULLUP); // Sets pin input with pullup resistor
  attachInterrupt(digitalPinToInterrupt(Gas_Counter_Pin), handleInterrupt, FALLING); // Attaches pin to interrupt on falling edge

  // Init serial
  Serial.begin(115200);
  Serial.println("Start");

  // Start WLAN
  Serial.println("Start WLAN Client DHCP");       // WiFi Mode Client with DHCP
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {         // Check connection
    wifi_retry++;
    delay(500);
    Serial.print(".");
    if (wifi_retry > 10) {
      Serial.println("\nReboot");                 // Reboot after 10 connection tries
      ESP.restart();
    }
  }  
  Serial.println();
  Serial.println(WiFi.localIP());

#if USE_MQTT == true
  // Start MQTT
  randomSeed(micros());
  client.setServer(mqtt_server, 1883);
#endif

  // Handle HTTP request events

  server.on("/", Event_Index);
  server.on("/set_data", Event_SetGasCount);
  server.on("/get_data", Event_GetGasCount);
  server.on("/reset", Event_ResetCount);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server started");

  ArduinoOTA.setHostname("Gascounter"); // Arduino OTA config and start
  ArduinoOTA.begin();

  configTzTime(TZ_INFO, NTP_SERVER);    // Synchronise system time with NTP
  getLocalTime(&local, 10000);          // Try  10 seconds
}

#if USE_MQTT == true
//*****************************************************************************
// Check MQTT connection an reconnect if necessary

void check_mqtt_connection()
{
  static unsigned long last_time = 0;

  if (!client.connected() && (millis() - last_time > 6000))  // Try reconnect every 6 seconds
  {
    last_time = millis();

    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Gascounter-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      //once connected to MQTT broker, subscribe command if any
      // client.subscribe("OsoyooCommand");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}
#endif


//*****************************************************************************
void Event_Index() {                         // If "http://<ip address>/" requested
  server.send(200, "text/html", MAIN_page);  // Send Index Website
}


//*****************************************************************************
// Add minute value to ring buffer

void addMinuteValue(void) {
  MinuteValue[MinutePtr] = DayCounter;
  MinutePtr++;
  if (MinutePtr > 59) {
    MinutePtr = 0;
    FullHour = true;
  }
}


//*****************************************************************************
// Get hour value from ring buffer

double getM3hValue(void) {
  unsigned CurrentPtr = 0;

  CurrentPtr = MinutePtr - 1;
  if (CurrentPtr < 0) CurrentPtr = 59;
  return (MinuteValue[CurrentPtr] - MinuteValue[MinutePtr]);
}


//*****************************************************************************
// Calculate values and send to web client

void Event_GetGasCount() {
  char buf[30];
  String Text;
  struct tm local;
  StaticJsonDocument<400> root;

  getLocalTime(&local, 100);
  if (!FullHour) {
    m3h = DeltaCounter / ((millis() - reset_time) / 1000) * 3600.0;
  } else {
    m3h = getM3hValue();
  }

  noInterrupts();
  if (PeriodCount != 0) m3h_akt = (GasDelta / (PeriodCount / 1000)) * 3600.0; // PeriodCount in 0.001 of a second

  if (millis() - Last_int_time > 60000) m3h_akt = 0;          // No signals m3h=0;
  interrupts();

  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", local.tm_hour, local.tm_min, local.tm_sec );
  root["time"] = buf;

  snprintf(buf, sizeof(buf), "%08.2f", GasCounter);
  root["stand"] = buf;

  snprintf(buf, sizeof(buf), "%05.2f", DayCounter);
  root["heute"] = buf;

  snprintf(buf, sizeof(buf), "%05.2f", DayBeforeCounter);
  root["gestern"] = buf;

  snprintf(buf, sizeof(buf), "%05.2f", m3h_akt);
  root["m3h_akt"] = buf;

  snprintf(buf, sizeof(buf), "%05.2f", m3h);
  root["m3h"] = buf;

  snprintf(buf, sizeof(buf), "%05.2f", DeltaCounter / ((millis() - reset_time) / 1000) * 3600.0 * 24.0);
  root["tag"] = buf;

  serializeJsonPretty(root, Text);
  server.send(200, "text/plain", Text); //Send values to client ajax request

}


//*****************************************************************************
// Set gas counter value

void Event_SetGasCount() {

  if (server.args() > 0) {
    GasCounter = server.arg(0).toFloat();
    GasCounter = (float)((int)(GasCounter * 100)) / 100.0;
  }
  server.send(200, "text/html");
}


//*****************************************************************************
// Reset gas counter to zero

void Event_ResetCount() {
  DeltaCounter = 0;
  m3h = 0;
  reset_time = millis();
  server.send(200, "text/html");
}


//*****************************************************************************
// No page

void handleNotFound() {
  server.send(404, "text/plain", "File Not Found\n\n");
}


//*****************************************************************************
// Get local time

bool getLocalTime(struct tm * info, uint32_t ms)
{
  uint32_t start = millis();
  time_t now;

  while ((millis() - start) <= ms) {
    time(&now);
    localtime_r(&now, info);
    if (info->tm_year > (2016 - 1900)) {
      return true;
    }
    delay(10);
  }
  return false;
}


//*****************************************************************************

void loop() {
  struct tm local;
  static unsigned long timer = 0;
  static unsigned long lastMsg = 0;
  static double lastGasCounter = 0;
  static bool lock = false;
  static int old_min = -1;
  char Text[20];

  server.handleClient();
  ArduinoOTA.handle();

#if USE_MQTT == true
  client.loop();
  check_mqtt_connection();
#endif

  if (millis() - timer > 10000) {   // Synchronise local time ever 10 seconds
    getLocalTime(&local, 10000);
    timer = millis();

    if (old_min == -1) {
      old_min = local.tm_min;       // Set minute value only once
    }

    // Hande day roll over
    if (local.tm_hour == 0 && local.tm_min == 0 && lock == false) {
      DayBeforeCounter = DayCounter;
      DayEndCounter = GasCounter;
      DayCounter = 0;
      lock = true;
    }

    if (local.tm_hour == 0 && local.tm_min == 1 && lock == true) {
      lock = false;
    }

    if (local.tm_min != old_min) {  // New minute
      addMinuteValue();             // Add value to ring buffer
      old_min = local.tm_min;
    }
  }

#if USE_MQTT == true

  // Handle MQTT publising of data
  // Send every minute or if counter changed

  if (client.connected() && ((millis() - lastMsg > 60000) || (GasCounter != lastGasCounter))) {
    lastMsg = millis();
    lastGasCounter = GasCounter;

    snprintf(Text, sizeof(Text), "%8.2f", GasCounter);
    client.publish("Count", Text);
    snprintf(Text, sizeof(Text), "%5.2f", DayCounter);
    client.publish("Today", Text);
    snprintf(Text, sizeof(Text), "%5.2f", DayBeforeCounter);
    client.publish("Yesterday", Text);
    snprintf(Text, sizeof(Text), "%8.2f", DayEndCounter);
    client.publish("DayEndCount", Text);
    snprintf(Text, sizeof(Text), "%5.2f", m3h_akt);
    client.publish("Current", Text);
    snprintf(Text, sizeof(Text), "%5.2f", m3h);
    client.publish("Hour", Text);
  }
#endif
}
