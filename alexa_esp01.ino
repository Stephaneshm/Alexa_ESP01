#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
//#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "fauxmoESP.h"  // version 3.10

// Rename the credentials.sample.h file to credentials.h and 
// edit it according to your router configuration
#include "credentials.h"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200

//#define LED_YELLOW          4
//#define LED_GREEN           5
//#define LED_STATUS         12
//#define LED_WIFI           LED_BUILTIN
#define LED_PINK            2
//#define LED_WHITE           15
//
//#define ID_YELLOW           "yellow lamp"
//#define ID_GREEN            "prise verte"
//#define ID_BLUE             "prise imprimante"
#define ID_PINK             "bureau"
//#define ID_WHITE            "white lamp"



void wifiSetup() {
    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);
    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
//        digitalWrite(LED_WIFI, LOW);
//        delay(500);
//        digitalWrite(LED_WIFI, HIGH);
        delay(600);
   }
    Serial.println();
    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }

void setup() {

    pinMode(LED_PINK, OUTPUT);
    digitalWrite(LED_PINK, LOW);
    delay(5000);
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("start");
    Serial.println();

    // Wifi
    wifiSetup();
    //WiFiManager wifiManager;
    //wifiManager.setTimeout(180);
    //if(!wifiManager.autoConnect("AutoConnectAP"))
     //{
     //   Serial.println("failed to connect and hit timeout");
     //   delay(3000);
     //   //reset and try again, or maybe put it to deep sleep
     //   ESP.reset();
     //   delay(5000);
    // } 
    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()

    delay(5000);
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
//    fauxmo.addDevice(ID_YELLOW);
//    fauxmo.addDevice(ID_GREEN);
//    fauxmo.addDevice(ID_BLUE);
    fauxmo.addDevice(ID_PINK);
//    fauxmo.addDevice(ID_WHITE);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.


//        if (strcmp(device_name, ID_YELLOW)==0) {
//            digitalWrite(LED_YELLOW, state ? HIGH : LOW);
//        } else if (strcmp(device_name, ID_GREEN)==0) {
//            digitalWrite(LED_GREEN, state ? HIGH : LOW);
//        } else if (strcmp(device_name, ID_BLUE)==0) {
//            digitalWrite(LED_BLUE, state ? HIGH : LOW);
//        } else
        if (strcmp(device_name, ID_PINK)==0) {
            digitalWrite(LED_PINK, state ? HIGH : LOW  );  
//            digitalWrite(LED_STATUS, state ? HIGH : LOW);
          }
//        else if (strcmp(device_name, ID_WHITE)==0) {
//            digitalWrite(LED_WHITE, state ? HIGH : LOW);
//        }
//
   });

}

void loop() {

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_YELLOW, true, 255);

}
