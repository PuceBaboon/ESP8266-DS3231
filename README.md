# ESP8266-DS3231
### Using a DS3231 to supply power to an ESP8266 ("no-power" deep sleep)

Uses the interrupt (alarm) from a DS3231 RTC chip to turn on a P-channel MOSFET, which supplies power to the ESP8266.

Requires:- https://github.com/rodan/ds3231

The ESP8266 reads the time and temperature from the DS3231 (and optionally does something useful with it), calculates the timing of the next wake-up call and loads it back to the DS3231 register, causing the DS3231 to turn off the power and send everything back into no-power sleep mode.  The DS3231 at this point is sipping power from it's own button-cell back-up battery.

Rinse and repeat.

### Update
Brian Robbins (https://github.com/bprobbins) Has very kindly provided a schematic and what looks like a readily available, modern P-MOSFET type for the actual power-switching circuit (it would probably only have taken me about another 8 or 9 years to around to it).  Check under the "Issues" menu above and scroll down to the -second- schematic.  Thanks, Brian!
