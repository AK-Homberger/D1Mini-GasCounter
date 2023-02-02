# D1 Mini Gas Counter

A simple ESP8266 based (D1 Mini) counter for the gas meter with a web interface. It is measuring pulses from the gas meter and calculating consumption values which are shown on a web interface. It's available in English/German language.

![Counter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Webclient_en.png)

![Counter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Webclient_de.png)

Depending on your gas meter type it might be sufficient to use a reed contact switch. For my meter [Pipersberg G4 RF1 c](https://www.pipersberg.de/wp-content/uploads/2019/12/Gas_81_Daten-Gaszaehler-RF1c-G4G6.pdf) it was necesssary to use an inductive sensor [LJ18A3-8-Z/BX](https://www.roboter-bausatz.de/p/lj18a3-8-z-bx-8mm-induktiver-naeherungssensor-npn-no-switch-dc-6-36v-300ma).

With the web interface you can set the current metering value. After that, it counts the individual pulses from the meter (0.01 m^3).
It shows the overall consumption, the currrent day consumption, the consumption from yesterday and the current consumption.

In addition the consumption fom the last hour and the total average consumption is shown.

If the client supports MDNS (Bonjour) you can access the server with "gascounter.local". If not you have to use the IP-Address.

![Meter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Meter.jpg)

Within the scetch you can select the language by coosing the desired header file, the timezone and set the wlan credentials.
That's all.

```
//#include "index_html_de.h"    // Web site information for Gauge / Buttons in German
#include "index_html_en.h"      // Web site information for Gauge / Buttons in English

#define TIME_24_HOUR      true
#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time

// Wifi
const char *ssid = "ssid";           // Set WLAN name
const char *password = "password";   // Set password
```

An additinal library (ArduinoJson) has to be installed with the library manager in the Arduino IDE.

The sensor is connected to pin D5 on the D1 Mini. The sensor (LJ18A3-8-Z/BX) is supplied with 12 Volt (6-36 Volt). For the D1 Mini you need the an additional step-down converter to create 5 Volt from the 12 Volt. With a reed contact you can directly use 5 Volt power supply.
