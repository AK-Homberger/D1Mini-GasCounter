# D1 Mini Gas Counter

A simple counter for the gas meter with a web interface. It is measuring pulses from the gas meter and calculating consuption values which are shown on a web interface.

![Counter-German](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Gascounter.png)

Depending on your gas meter type it might be sufficient to use a reed contcat switch. For my meter (Pipersberg G4 RF1 c)it was necesssary to use an inductive sensor (LJ18A3-8-Z/BX).

With the web interface you can set the current metering value. After that it counts the individial pulses from the meter (0.1 m^3).
It shows the overall consuption, the currrent day consuption, the consumption from yesterday and the current consumption.

In addition the consuption fom the last hour and the total average consumption is shown.

![Meter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Meter.jpg)

Within the scetch you can select the language by coosing the desired web page, the timezone and set the wlan credentials.
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

The sensor is connected to pin D5 on the D1 Mini. The sensor (LJ18A3-8-Z/BX) requires 12 Volt. For the D1 Mini you need the an additional step-down converter to create 5 Volt from the 12 Volt. With a reed contact you can directly use 5 Volt power supply.
