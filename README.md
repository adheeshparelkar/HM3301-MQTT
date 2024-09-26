# HM3301-MQTT
ESP8266 MQTT For HM3301 Air Quality Sensors
Refer for the the pinout diagram for your board to determine SDA and SCL pins. For WemosD1Mini the pins are D1 (SCL) and D2 (SDA)
The HM3301 I used requies 5V which I got directly from the WemosD1Mini.
The four connections are SCL, SDA, 5V and GND
Edit the Wifi SSID, Wifi Password, MQTT Server Address, MQTT Username and MQTT Password in mqtt.ino
Upload the sketch.
To troubleshoot open serial monitor at 9600bps to check status/errors.
