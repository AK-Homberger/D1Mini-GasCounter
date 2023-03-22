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

// DHT11 Tem/Hum MQTT Sender for ESP8266 (e.g. D1-Mini)
// Version 1.0 - 22.03.2023

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Two additional libraries have to be installed.
// "PubSubClient" library to be installed with Library Manager

// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

// Connect pin 1 (on the left) of the sensor to +3,3V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND

#define DHTPIN 12     // Digital pin connected to the DHT sensor GPIO12 / D6

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Update these with values suitable for your network.
const char* ssid = "ssid";
const char* password = "password";

// Set IP of MQTT server and MQTT Client ID
const char* mqtt_server = "192.168.0.71";
String clientId = "DHT11-Sender";


WiFiClient espClient;
PubSubClient MQTT_client(espClient);
DHT dht(DHTPIN, DHTTYPE);

//*******************************************************************************
//
void setup() {
  Serial.begin(115200);
  while (!Serial);

  dht.begin();
  
  // We start connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  MQTT_client.setServer(mqtt_server, 1883);
}


//*****************************************************************************
// Check MQTT connection an reconnect if necessary

void check_mqtt_connection()
{
  static unsigned long last_time = 0;

  MQTT_client.loop();

  if (!MQTT_client.connected() && (millis() - last_time > 6000))  // Try reconnect every 6 seconds
  {
    last_time = millis();

    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (MQTT_client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      //once connected to MQTT broker, subscribe command if any
      // client.subscribe("XXXCommand");
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTT_client.state());
      Serial.println(" try again in 6 seconds");
    }
  }
}


//*******************************************************************************
//
void loop() {
  static unsigned long lastMsg = 0;
  char Text[20];
  float Temp = 0, Humidity = 0;

  check_mqtt_connection();

  // Read DHT sensor and send every 5 seconds

  if (MQTT_client.connected() && millis() - lastMsg > 5000) {
    lastMsg = millis();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  
    Temp = dht.readTemperature();   // Read Temperature
    Humidity = dht.readHumidity();  // Read Humidity

    Serial.println("Publish values.");

    // Publish formatted values
    snprintf(Text, sizeof(Text), "%4.1f", Temp);
    MQTT_client.publish("InsideTemperature", Text);

    snprintf(Text, sizeof(Text), "%3.0f", Humidity);
    MQTT_client.publish("InsideHumidity", Text);
  }
}
