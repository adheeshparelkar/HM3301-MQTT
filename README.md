# HM3301-MQTT
ESP8266 MQTT For HM3301 Air Quality Sensors
1. Refer for the the pinout diagram for your board to determine SDA and SCL pins. For WemosD1Mini the pins are D1 (SCL) and D2 (SDA)
2. The HM3301 I used requies 5V which I got directly from the WemosD1Mini.
3. The four connections are SCL, SDA, 5V and GND
4. Edit the Wifi SSID, Wifi Password, MQTT Server Address, MQTT Username and MQTT Password in HM3301_MQTT.ino
5. Upload the sketch.
6. To troubleshoot open serial monitor at 115200bps to check status/errors.
