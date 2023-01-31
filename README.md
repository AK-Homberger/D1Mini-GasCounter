# D1-Mini Gas Counter

A simple counter for the gas meter with a web interface.

![Counter-German](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Gascounter.png)

Depending on jour gas meter type it might be sufficient to use a reed contcat switch. For my meter (Pipersberg G4 RF1 c)it was necesssary to use an inductive sensor (LJ18A3-8-Z/BX).

![Meter](https://github.com/AK-Homberger/D1Mini-GasCounter/blob/main/Meter.jpg)

Within the scetch you can 

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
