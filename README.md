# D1 Mini Gas Counter

A simple ESP8266 based (D1 Mini) counter for the gas meter with a web interface. It is measuring pulses from the gas meter and calculating consumption values which are shown on a web interface. It's available in English/German language. MQTT and ioBroker integration is supported as an option.

![Counter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Webclient_en.png)

![Counter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Webclient_de.png)

Depending on your gas meter type, it might be sufficient to use a reed contact switch. For my meter [Pipersberg G4 RF1 c](https://www.pipersberg.de/wp-content/uploads/2019/12/Gas_81_Daten-Gaszaehler-RF1c-G4G6.pdf) it was necesssary to use an inductive sensor [LJ18A3-8-Z/BX](https://www.roboter-bausatz.de/p/lj18a3-8-z-bx-8mm-induktiver-naeherungssensor-npn-no-switch-dc-6-36v-300ma).

With the web interface you can set the current metering value. After that, it counts the individual pulses from the meter (0.01 m^3).
It shows the overall consumption, the currrent day consumption, the consumption from yesterday and the current consumption.

In addition, the consumption fom the last hour and the total daily average consumption is shown.

If the client supports MDNS (Bonjour), you can access the server with "gascounter.local". If not you have to use the IP-Address.

![Meter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Meter.jpg)

# Hardware
The open collector output from the sensor is connected to pin D5 on the D1 Mini. No additional resistor is necessary due to the configured internal pull-up resistor for the D5 input pin.

The sensor (LJ18A3-8-Z/BX) is supplied with 12 Volt (6-36 Volt). For the D1 Mini you need an additional [step-down converter](https://eckstein-shop.de/MiniDC-DCStep-downSpannungsreglerMP1584ENBuckPowerModuleOutout02C8-20V3A) to create 5 Volt from the 12 Volt. With a reed contact you can directly use 5 Volt power supply.

A more detailled connection diagram will follow soon.

# Software
The Arduino IDE has to be installed and the ESP8266 Board URL "https://arduino.esp8266.com/stable/package_esp8266com_index.json" has to be included in the preferences.

Two additional libraries have to be installed with the library manager in the Arduino IDE:

- ArduinoJson
- PubSubClient

Within the scetch you can select the language by coosing the desired header file, the timezone and set the wlan credentials.
If you want to publish data via MQTT then set "USE_MQTT" to "true" otherwise to "false". The MQTT server IP can be defined with the "mqtt_server" variable.

```
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
```
I'm using MQTT to publish data to an [ioBroker](https://www.iobroker.net/) instance with an [influxDB](https://github.com/influxdata/influxdb#readme) database on a Raspberry server. That provides an easy was to log consumption data and to visualise the consumption history.

![ioBroker](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/ioBroker.png)

![VIS](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/ioBroker-VIS.png)

Detailled instructions for installation and configuration of ioBroker and influxdb will follow soon...

The visualisation is available [here](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/VIS-Project.zip). Just import it with the VIS editor. The visulisation is using, in addition to the gas values, also inside/outside temperature and humidity. One option to get this information into ioBroker is a receiver/decoder for 433MHz sensors. 

An example, especially for a Bresser 3 channel sensor, can be found [here](https://github.com/AK-Homberger/Bresser-3CH-433MHz-T-H-Sensor-decoder). 

But integration with an ESP8266 plus [BME280](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/BME280-MQTT-Sender/BME280-MQTT-Sender.ino) or [DHT11](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/DHT11-MQTT-Sender/DHT11-MQTT-Sender.ino) via MQTT is another option. A sketch for both options is avalable with the links above.

If you are using the Bresser sensor for outside Temp/Hum and one of the others (BME280/DHT11) for inside Temp/Hum the visualisation will work out of the box, without any changes needed in object names. For the MQTT objects, which are created automatically, it might be necessary to change the "role" of the object to "variable" and "type" to "number". 

For the Bresser sensor, everything is prepared correctly from the integration script.

For a corretly working visualisation, please make sure to enable Influxdb logging in ioBroker for these objects:

- javascript.0.Temperature
- mqtt.0.InsideTemperature
- mqtt.0.Current
- mqtt.0.DayEndCount
- mqtt.0.Yesterday

The standard setting "only store changes" is sufficient.

The visualisation for the Bresser sensor supports also the Low Battery indicator in VIS. That means a blinking battery symbol is shown right to the humidity value, to indicate that the sensor is signaling a needed battery replacement.

# Parts:

- D1 Mini [Reichelt](https://www.reichelt.de/de/en/d1-mini-esp8266-v3-0-d1-mini-p253978.html?&nbc=1)
- D1 Mini [AzDelivery](https://www.az-delivery.de/en/products/d1-mini?variant=28983931346&utm_source=google&utm_medium=cpc&utm_campaign=19229855661&utm_content=147170319769&utm_term=&gclid=CjwKCAiA3KefBhByEiwAi2LDHM7r8K3XCFbBlfJzONqn45p8F_Yk5XTZbuT6FBbNv-6jXnv12LEDWhoC-ZQQAvD_BwE)
- Sensor [RoboterBausatz](https://www.roboter-bausatz.de/p/lj18a3-8-z-bx-8mm-induktiver-naeherungssensor-npn-no-switch-dc-6-36v-300ma)
- Reed Contact [Reichelt](https://www.reichelt.de/de/en/reed-sensor-170-v-0-5-a-normally-open-mk06-4c-p151832.html?&trstct=pos_11&nbc=1)
- Step-Down-Converter [Eckstein](https://eckstein-shop.de/MiniDC-DCStep-downSpannungsreglerMP1584ENBuckPowerModuleOutout02C8-20V3A)
- Power Supply 5V [Reichelt](https://www.reichelt.de/de/en/eco-friendly-plug-in-power-supply-unit-5-v-1000-ma-2-1-mm-snt-1000-5v-p111180.html?GROUPID=4946&START=0&OFFSET=100&SID=94f94f8fe09e887944479f1c1d0a4fcf48b8583b9cb9a4e3de5a0&LANGUAGE=EN&&r=1)
- Power Supply 12 Volt [Reichelt](https://www.reichelt.de/de/en/eco-friendly-plug-in-power-supply-unit-12-v-600-ma-2-5-mm-snt-600-12v-2-5-p108992.html?&nbc=1)

# Updates:
- Version 1.4 - 20.02.230: Calculate values before MQTT send.
- Version 1.3 - 13.02.230: Debouncing interrupt as preparation for reed contact use.
- Version 1.1 - 09.02.230: Added MQTT option.
