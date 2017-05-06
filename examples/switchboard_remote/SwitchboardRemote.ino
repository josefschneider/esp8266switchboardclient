
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266SwitchboardClient.h>

#include "TemperatureSensor.h"
#include "IRSender.h"

const char* ssid     = "SSID";
const char* password = "PASSWORD";

ESP8266WebServer server(80);
ESP8266SwitchboardClient<800, 400> swbClient(server);

// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01
#define DHTPIN  4
TemperatureSensor tempSensor(DHTPIN, DHT22, 11); // 11 works fine for ESP8266

#define IR_REMOTE_PIN 2
IRSender irSender(IR_REMOTE_PIN);

void setup() {
    Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable

    swbClient.AddDevice(tempSensor);
    swbClient.AddDevice(irSender);
    if (swbClient.Error())
        Serial.println("\n\r\n\rError: " + swbClient.GetErrorString());

    // Connect to WiFi network
    WiFi.begin(ssid, password);
    Serial.print("\n\rWorking to connect");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Switchboard Client");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

