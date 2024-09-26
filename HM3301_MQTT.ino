#include <Seeed_HM330X.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL_OUTPUT SerialUSB
#else
    #define SERIAL_OUTPUT Serial
#endif

HM330X sensor;
uint8_t buf[30];

// WiFi and MQTT configuration
const char* ssid = "WIFI-SSID"; // Replace with your WiFi SSID
const char* password = "WIFIPASSWORD"; // Replace with your WiFi password
const char* mqttServer = "<MQTT-BROKER-ADDRESS>"; // Replace with your MQTT broker address
const int mqttPort = 1883; // Replace with your MQTT broker port
const char* mqttTopic = "<MQTT-TOPIC>"; // Topic to publish PM2.5 values

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* str[] = {
    "sensor num: ", 
    "PM1.0 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
    "PM2.5 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
    "PM10 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
    "PM1.0 concentration(Atmospheric environment,unit:ug/m3): ",
    "PM2.5 concentration(Atmospheric environment,unit:ug/m3): ",
    "PM10 concentration(Atmospheric environment,unit:ug/m3): ",
};

HM330XErrorCode print_result(const char* str, uint16_t value) {
    if (NULL == str) {
        return ERROR_PARAM;
    }
    SERIAL_OUTPUT.print(str);
    SERIAL_OUTPUT.println(value);
    return NO_ERROR;
}

HM330XErrorCode parse_result(uint8_t* data) {
    uint16_t value = 0;
    if (NULL == data) {
        return ERROR_PARAM;
    }
    for (int i = 1; i < 8; i++) {
        value = (uint16_t) data[i * 2] << 8 | data[i * 2 + 1];
        print_result(str[i - 1], value);
        
        // Publish PM2.5 values to MQTT
        if (i == 3) { // PM2.5 concentration is at index 3
            char payload[50];
            snprintf(payload, sizeof(payload), "{\"pm25\": %d}", value);
            mqttClient.publish(mqttTopic, payload);
            SERIAL_OUTPUT.println("Published PM2.5 value to MQTT: " + String(value));
        }
    }

    return NO_ERROR;
}

HM330XErrorCode parse_result_value(uint8_t* data) {
    if (NULL == data) {
        return ERROR_PARAM;
    }
    for (int i = 0; i < 28; i++) {
        SERIAL_OUTPUT.print(data[i], HEX);
        SERIAL_OUTPUT.print("  ");
        if ((0 == (i) % 5) || (0 == i)) {
            SERIAL_OUTPUT.println("");
        }
    }
    uint8_t sum = 0;
    for (int i = 0; i < 28; i++) {
        sum += data[i];
    }
    if (sum != data[28]) {
        SERIAL_OUTPUT.println("wrong checkSum!!");
    }
    SERIAL_OUTPUT.println("");
    return NO_ERROR;
}

void setup() {
    SERIAL_OUTPUT.begin(115200);
    delay(100);
    SERIAL_OUTPUT.println("Serial start");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        SERIAL_OUTPUT.println("Connecting to WiFi...");
    }
    SERIAL_OUTPUT.println("Connected to WiFi");

    // Setup MQTT client
    mqttClient.setServer(mqttServer, mqttPort);

    if (sensor.init()) {
        SERIAL_OUTPUT.println("HM330X init failed!!");
        while (1);
    }
}

void loop() {
    if (!mqttClient.connected()) {
        // Reconnect to MQTT
        while (!mqttClient.connected()) {
            SERIAL_OUTPUT.println("Attempting MQTT connection...");
            if (mqttClient.connect("HM330XClient")) { // Use a unique client ID
                SERIAL_OUTPUT.println("MQTT connected");
            } else {
                SERIAL_OUTPUT.print("Failed with state ");
                SERIAL_OUTPUT.println(mqttClient.state());
                delay(2000);
            }
        }
    }
    mqttClient.loop(); // Keep MQTT connection alive

    if (sensor.read_sensor_value(buf, 29)) {
        SERIAL_OUTPUT.println("HM330X read result failed!!");
    }
    parse_result_value(buf);
    parse_result(buf);
    SERIAL_OUTPUT.println("");
    delay(5000);
}
