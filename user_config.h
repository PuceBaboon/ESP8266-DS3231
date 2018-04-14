/*
 * $Id: user_config.h,v 1.1 2016/11/03 00:47:20 anoncvs Exp $
 *
 * Configuration for local set-up (ie:- Access-point ID & PWD).
 */

#define DEBUG		0

#define MQTT_HOST	"mqtt.hostname.com"	/* !! Change Me !! */
#define MQTT_PORT	1883
#define MQTT_BUF_SIZE	1024
#define MQTT_KEEPALIVE	120	 /* In seconds. */

#define MQTT_CLIENT_ID	"Puce_%08X"
#define MQTT_USER	"Puce_USER"
#define MQTT_PASS	"Puce_PASS"


#define STA_SSID	"XXX_XXX"	/* !! Change Me !! */
#define STA_PASS	"XXX_XXX"	/* !! Change Me !! */
#define STA_TYPE	AUTH_WPA2_PSK

#define MQTT_RECONNECT_TIMEOUT 	5	/* In Seconds. */

#define CLIENT_SSL_ENABLE
#define DEFAULT_SECURITY	0

#define QUEUE_BUFFER_SIZE 	1024
#define RTC_BUFF_SIZE		256

#define ALM2_SLEEP_MINS		2

// I2C GPIO Defines.
#define I2C_SDA	2
#define I2C_SCL	14

