# ESP8266-DS3231
### Using a DS3231 to control power to an ESP8266 ("no-power" deep sleep)

Uses the interrupt (alarm) from a DS3231 RTC chip to turn on a P-channel MOSFET, which supplies power to the ESP8266.

The ESP8266 reads the time and temperature from the DS3231 (and optionally does something useful with it), calculates the timing of the next wake-up call and loads it back to the DS3231 register, causing the DS3231 to turn off the power and send everything back into no-power sleep mode.  The DS3231 at this point is sipping power from it's own button-cell back-up battery.

Rinse and repeat.

![Schematic of GPIO/DS3231-controlled switch](./ESP_Power_Latch.png)

#### Hardware Notes

The switch connections shown at the top of the board are there to enable a push-button switch to be added to the project for development and updating (otherwise you have to wait for the DS3231 to wake up the ESP8266 before you can update the firmware).

The 2N7000 N-channel MOSFET is there to protect the ESP8266 (a 3v3 device) against the high battery voltage present at the lower end of the R1/R3 resistor chain when the ESP8266 is off (which is most of the time).  Yes, I know that it presents a very small current and that many people claim that the ESP8266 is 5v tolerant anyway, but this is belt-and-braces protection for something which is projected to have a long usage life (a version of this has been running for many years now as a single-button remote on/off switch for our ancient television -- and it gets more than two years of battery life while still being used a minimum of six times per day and despite having multiple blinkenlights to help diagnose any problems).

The R1/R3 resistor pair were sized to prevent an excessively low gate voltage on the AO3415 P-MOSFET.  You might need to change these values if you replace that device with some other type.

The pads labelled ENAB1/ENAB2 on the bottom left are the connections from the DS3231, which switch the power to the ESP8266 "on" when the DS3231 timer expires and the -INT signal goes low.  The DS3231 doesn't require a positive supply from the ESP8266 side of the power switch, as it is modified to run -only- of the CR2032 button cell (see the photo of the DS3231, which has pin-2 removed).  If you want to use the 4kB AT24C32 EEPROM on the DS3231 board, you can supply a 3v3 line to the DS3231 module and it will work quite happily (note that when the ESP8266 shuts down, so does the AT24C32, but the DS3231 still ticks along on it's own coin-cell battery).


#### Software Notes

 (Currently) Requires:- https://github.com/rodan/ds3231
 The other (parallel) projects using this hardware are now using the Adafruit RTClib library.  This project will be updated to reflect that in the very near future [written Feb 2022].


### Updates

A few years back, Brian Robbins (https://github.com/bprobbins) very kindly provided a schematic and what looks like a readily available, modern P-MOSFET type for the actual power-switching circuit (it would probably only have taken me about another 8 or 9 years to get around to it).  Check under the "Issues" menu above and scroll down to the -second- schematic.  Thanks, Brian!   a
  --  ...and yes, it did take me years, but Brian's (great!) schematic has now been replaced with one to make it a little clearer where the physical connections fit together.  There's also an early (but working) version of the PCB for this circuit available from OSH Park's shared-projects area 
![OSH Park](<a href="https://oshpark.com/shared_projects/iFWjTLa5")
This is a mainly through-hole component board, with the single exception of the P-channel MOSFET, which is a surface-mount (but not too difficult to solder with a normal iron).  Sometime in the next 10 or so years, I might get around to updating this board to a more useful form-factor.
