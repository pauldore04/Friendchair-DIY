#ifndef WIFI_MQTT_MANAGER_H
#define WIFI_MQTT_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "state/ChairState.h"
#include "sensor/PressureSensor.h"
#include "input/StateButton.h"
#include "output/OutputManager.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

class WifiMQTTManager {
private:
    enum NetworkState {
        SETUP,      // WiFi/MQTT Setup
        CONNECTING, // Verbindungsaufbau mit Blink-LED
        CONNECTED   // Verbunden - Datenübertragung aktiv
    };
    
    // WiFi & MQTT Configuration (aus old.cpp)
    static const char* mqtt_server;
    static const int mqtt_port;
    static String espnumber;  // Jetzt konfigurierbar
    static const String projectPrefix;
    static int builtinLedPin;  // Konfigurierbare Built-in LED
    
    // WiFi & MQTT Clients
    static WiFiClient espClient;
    static PubSubClient mqttClient;
    static WiFiManager wifiManager;
    
    static ChairState* chairState;
    static PressureSensor* pressureSensor;
    static StateButton* stateButton;
    static OutputManager* outputManager;
    static bool isConnected;
    static bool isInitialized;
    static NetworkState currentState;
    static unsigned long lastDataTransmission;
    
    // Change Detection Variablen
    static bool lastSensorState;
    static int lastChairState;
    
    // Statische Task-Funktionen
    static void unifiedNetworkTask(void* parameter);
    
    // WiFi & MQTT Helper-Funktionen (aus old.cpp adaptiert)
    static void setupWiFi();
    static void setupMQTT();
    static void connectToMQTT();
    static void reconnectWiFi();
    static void mqttCallback(char* topic, byte* payload, unsigned int length);
    static void handleIncomingData();
    static void sendSensorData();
    static void sendButtonData();

public:
    WifiMQTTManager();
    void init(ChairState* state, PressureSensor* sensor, StateButton* button, OutputManager* output, 
              const String& espNumber, int builtinLed);
    void startTasks();
    bool getConnectionStatus() const;
};

#endif
