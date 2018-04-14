/*
 *   $Id: DS3231.ino,v 1.4 2016/11/03 13:22:59 anoncvs Exp $
 *
 * Locally modified version for testing basic hardware functionality.
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "user_config.h"
#include <Wire.h>
#include <ds3231.h>

uint8_t alarm_sleep_mins = ALM2_SLEEP_MINS;	// Time between alarms (set in user_config.h).

// how often to refresh the info on stdout (ms)
unsigned long prev = 30000, interval = 30000;

void set_next_alarm(void) {
    struct ts t;
    unsigned char wakeup_min;

    DS3231_get(&t);		// Grab the current time from the DS3231.

    /*
     * Calculate when our next minute alarm will occur, based
     * on the current time.
     */
    wakeup_min = (t.min / alarm_sleep_mins + 1) * alarm_sleep_mins;
    if (wakeup_min > 59) {
	wakeup_min -= 60;
    }

    /*
     * These flag settings determine which fields will be checked.
     *          A2M2 (Minutes)  (0 = enable).
     *          A2M3 (Hours)    (1 = disable). 
     *          A2M4 (Days)     (1 = disable).
     *          DY/DT           (1 = day-of-week, 0 = day-of-month).
     */
    uint8_t flags[4] = { 0, 1, 1, 1 };	// So, check against minutes field only.

    /* 
     * Set Alarm2. Only the minutes field is set, as the flags (above)
     * ensure that the other fields are ignored anyway.
     */
    DS3231_set_a2(wakeup_min, 0, 0, flags);

    /*
     * ...and enable.
     */
    DS3231_set_creg(DS3231_INTCN | DS3231_A2IE);
}


/*
 * Unlike standard Arduino programs, virtually all of the action here happens in the setup()
 * function.  Our hardware consists of the ESP8266 module connected via I2C to a DS3231 RTC
 * module.  The DS3231 provides two essential functions.  The first is the actual temperature
 * measurement; we use the DS3231 on-chip sensor to provide temperature readings of the
 * ambient air temperature (it's accurate enough for our purposes).  The second function is
 * control of DC power to all of the electronics (including the DS3231 chip itself).  This is
 * implemented using the open-drain "INT" output of the DS3231 to control a P-channel MOSFET
 * in the +ve line from the battery pack.  When an alarm goes off, the INT line is actively
 * pulled low, taking the gate of the MOSFET low and switching it hard on.  This provides
 * main battery power, via a low-drop-out 3v3 regulator, to the ESP8266 and the DS3231.
 *
 * The setup() function is called at power on.  We want it to call all of the functions
 * which this particular application performs in a linear manner and then to power-off,
 * without dropping into the loop() function.
 *
 * BASIC FUNCTION
 *
 *	Initialize.
 *	Get temperature reading from DS3231.
 *	Initialize network.
 *	Initialize MQTT.
 *	Send temperature and timestamp.
 *
 */
void setup() {
    float tempr;
    char buff[RTC_BUFF_SIZE];

    Serial.begin(115200);
    Serial.println();		// <CR> past the start-up crap.
    Wire.begin(I2C_SDA, I2C_SCL);	// Set the I2C pins to:- 2=SDA, 14=SCL.

/* *INDENT-OFF* */
    /* OTA (Over The Air) code.  DO NOT DELETE!! */
    /* The OTA port defaults to 8266. */
    // ArduinoOTA.setPort(8266);

    /* The hostname defaults to esp8266-[ChipID]. */
    // ArduinoOTA.setHostname("myesp8266");

    /* No authentication by default. */
    // ArduinoOTA.setPassword((const char *)"123");

    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    /* End of OTA code. */
/* *INDENT-ON* */


    /* 
     * The "init" function actually just sets the INTCN bit, to disable
     * square wave O/P and enable interrupts on alarms.
     */
    DS3231_init(DS3231_INTCN);

    tempr = DS3231_get_treg();	// Grab the temperature from the DS3231.
    dtostrf(tempr, 4, 2, buff);
    Serial.print(F("Temperature: "));
    Serial.print(buff);
    Serial.println(F("C"));
    DS3231_clear_a2f();		// Our hardware adds a delay to switch-off at this point.
    set_next_alarm();		// ...so quickly go off and set the next alarm while we still have power.
}


/*
 * NOTE - Normally, our program should never get this far.  The setup() function
 *        contains all of the functionality for the temperature sensor and the
 *	  call to "set_next_alarm()" at the end of it should automatically 
 *	  remove power from the ESP8266.  It will be woken by the INT line of
 *	  the DS3231 going low when that alarm event time is reached (the DS3231
 *	  is powered by it's own 3v, coin-cell battery during the time when the
 *	  main power is off).
 *
 *	  If we do drop down into the loop() function, it probably means that the
 *	  alarm could not be set for some reason, so we should flag an error
 *	  using MQTT.
 *
 */
void loop() {
    char buff[RTC_BUFF_SIZE];
    unsigned long now = millis();
    struct ts t;

    ArduinoOTA.handle();        // Handle OTA requests.

    // Display the current time.
    if ((now - prev > interval) && (Serial.available() <= 0)) {
	DS3231_get(&t);

	snprintf(buff, RTC_BUFF_SIZE, "%d-%02d-%02d %02d:%02d:%02d",
		 t.year, t.mon, t.mday, t.hour, t.min, t.sec);
	Serial.println(buff);

#ifdef DEBUG
	// Display the Alarm2 register settings.
	DS3231_get_a2(&buff[0], 59);
	Serial.println(buff);
#endif

	if (DS3231_triggered_a2()) {
	    Serial.println(" -> alarm2 has been triggered");
	    set_next_alarm();
	    // clear a2 alarm flag and let INT go into hi-z
	    DS3231_clear_a2f();
	}
	prev = now;
    }
}
