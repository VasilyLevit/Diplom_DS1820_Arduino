//
#include <UIPEthernet.h>
#include <PubSubClient.h>

// Включаем библиотеки для чтения данных с датчика
#include <OneWire.h>
#include <DallasTemperature.h>

#define CLIENT_ID "ArduinoMQTT"
#define TOPIC "temperature"
#define PUBLISH_DELAY 5000

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
IPAddress mqttServer(10, 3, 14, 96);

EthernetClient ethClient;
PubSubClient mqttClient;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

long previousMillis;

/*
* The setup function. We only start the sensors here
*/
void setup() {

    // setup serial communication
    Serial.begin(9600);
    while (!Serial) {
    }
    Serial.println(F("MQTT Arduino Demo"));
    Serial.println();

    // setup ethernet communication using DHCP
    if (Ethernet.begin(mac) == 0) {
        Serial.println(F("Unable to configure Ethernet using DHCP"));
        for (;;);
    }
    Serial.println(F("Ethernet configured via DHCP"));
    Serial.print("IP address: ");
    Serial.println(Ethernet.localIP());
    Serial.println();

    // setup mqtt client
    mqttClient.setClient(ethClient);
    mqttClient.setServer(mqttServer, 1883);
    Serial.println(F("MQTT client configured"));

    // setup MS18B20 sensor
    Serial.println("Dallas Temperature IC Control Library Demo");

    // Start up the library
    sensors.begin();

    Serial.println();
    Serial.println(F("Ready to send data"));
    previousMillis = millis();
}

/*
* Main function, get and show the temperature
*/
void loop() {
    // it's time to send new data?
    if (millis() - previousMillis > PUBLISH_DELAY) {
        sendData();
        previousMillis = millis();
    }
    mqttClient.loop();
}

void sendData() {
    char msgBuffer[50];
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    float t = sensors.getTempCByIndex(0);
    Serial.print("Temperature: ");
    Serial.println(t);
    
    if (mqttClient.connect(CLIENT_ID)) {
        mqttClient.publish(TOPIC, dtostrf(t, 6, 2, msgBuffer));
    }
}