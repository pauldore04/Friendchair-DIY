#include "network/NetworkManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

// WiFi & MQTT Configuration (aus old.cpp)
const char* WifiMQTTManager::mqtt_server = "test.mosquitto.org";
const int WifiMQTTManager::mqtt_port = 1883;
String WifiMQTTManager::espnumber = "01";  // Wird in init() gesetzt
const String WifiMQTTManager::projectPrefix = "phf-iot-2025";
int WifiMQTTManager::builtinLedPin = 2;    // Wird in init() gesetzt

// WiFi & MQTT Clients
WiFiClient WifiMQTTManager::espClient;
PubSubClient WifiMQTTManager::mqttClient(espClient);
WiFiManager WifiMQTTManager::wifiManager;

// Zeiger Variablen
ChairState* WifiMQTTManager::chairState = nullptr;
PressureSensor* WifiMQTTManager::pressureSensor = nullptr;
StateButton* WifiMQTTManager::stateButton = nullptr;
OutputManager* WifiMQTTManager::outputManager = nullptr;
bool WifiMQTTManager::isConnected = false;
bool WifiMQTTManager::isInitialized = false;
WifiMQTTManager::NetworkState WifiMQTTManager::currentState = WifiMQTTManager::SETUP;
unsigned long WifiMQTTManager::lastDataTransmission = 0;

// Change Detection Variablen
bool WifiMQTTManager::lastSensorState = false;
int WifiMQTTManager::lastChairState = -1;

WifiMQTTManager::WifiMQTTManager() {
}

void WifiMQTTManager::init(ChairState* state, PressureSensor* sensor, StateButton* button, OutputManager* output, 
                         const String& espNumber, int builtinLed) {
    chairState = state;
    pressureSensor = sensor;
    stateButton = button;
    outputManager = output;
    
    // Konfigurierbare Parameter setzen
    espnumber = espNumber;
    builtinLedPin = builtinLed;
    
    // MQTT Server und Callback setzen (aus old.cpp)
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqttCallback);
    
    isInitialized = true;
    currentState = SETUP;
    Serial.printf("NetworkManager initialized - ESP: %s, Built-in LED: %d\n", 
                  espnumber.c_str(), builtinLedPin);
}

void WifiMQTTManager::startTasks() {
    if (!isInitialized) {
        Serial.println("ERROR: NetworkManager not initialized!");
        return;
    }
    
    // Ein einziger effizienter Task mit State-Machine
    xTaskCreate(
        unifiedNetworkTask,
        "UnifiedNetworkTask",
        4096,
        NULL,
        3,              // Hohe Priorität für Netzwerk
        NULL
    );
    
    Serial.println("NetworkManager unified task started (Priority 3)");
}

void WifiMQTTManager::unifiedNetworkTask(void* parameter) {
    const TickType_t setupDelay = pdMS_TO_TICKS(2000);
    const TickType_t connectionDelay = pdMS_TO_TICKS(3000);
    const TickType_t monitoringInterval = pdMS_TO_TICKS(1000);
    const unsigned long dataTransmissionInterval = 5000; // 5s in milliseconds
    
    while (true) {
        switch (currentState) {
            case SETUP:
                Serial.println("NetworkTask: Setting up WiFi...");
                setupWiFi();
                vTaskDelay(setupDelay);
                currentState = CONNECTING;
                break;
                
            case CONNECTING:
                Serial.println("NetworkTask: Connecting to WiFi...");
                outputManager->blinkInternalLed();
                vTaskDelay(connectionDelay);
                
                Serial.println("NetworkTask: Setting up MQTT...");
                setupMQTT();
                vTaskDelay(setupDelay);
                
                // Verbindung erfolgreich
                isConnected = true;
                currentState = CONNECTED;
                outputManager->setInternalLed(true);
                Serial.println("NetworkTask: Connection established!");
                lastDataTransmission = millis();
                break;
                
            case CONNECTED:
                // Eingehende Daten verarbeiten
                handleIncomingData();
                
                // Daten bei Änderungen sofort senden (Change Detection)
                sendSensorData();
                sendButtonData();
                
                // TODO: Verbindung überwachen - bei Verlust zurück zu CONNECTING
                // if (!isStillConnected()) currentState = CONNECTING;
                break;
        }
        
        vTaskDelay(monitoringInterval);
    }
}

void WifiMQTTManager::setupWiFi() {
    // WiFi-Setup aus old.cpp adaptiert
    Serial.println("Starting WiFi setup...");
    
    // WiFiManager für automatische Verbindung (aus old.cpp)
    if (!wifiManager.autoConnect((String("ESP32_") + espnumber).c_str())) {
        Serial.println("WLAN fehlgeschlagen, restart in 3s...");
        outputManager->setInternalLed(false);
        vTaskDelay(pdMS_TO_TICKS(3000));
        ESP.restart();
    }
    
    Serial.println("WLAN verbunden! IP: " + WiFi.localIP().toString());
}

void WifiMQTTManager::setupMQTT() {
    // MQTT-Setup - bereits in init() gesetzt, hier nur verbinden
    connectToMQTT();
}

// MQTT Callback aus old.cpp adaptiert
void WifiMQTTManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.println("📥 MQTT empfangen: " + message + " (Topic: " + String(topic) + ")");
    
    // LED-Steuerung über OutputManager (aus old.cpp adaptiert)
    String expectedTopic = projectPrefix + "/brightness";
    if (String(topic) == expectedTopic) {
        int brightness = message.toInt();
        brightness = constrain(brightness, 0, 255);
        
        // Externe LED über OutputManager steuern (0-255 -> true/false)
        if (outputManager != nullptr) {
            outputManager->setExternalLed(brightness > 0);
            Serial.printf("💡 LED gesetzt auf: %s (brightness: %d)\n", 
                         brightness > 0 ? "ON" : "OFF", brightness);
        }
    }
}

// Verbindet sich mit dem MQTT-Broker (aus old.cpp)
void WifiMQTTManager::connectToMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Verbinde zu MQTT...");
        String clientId = projectPrefix + "_ESP32_" + espnumber;
        
        if (mqttClient.connect(clientId.c_str())) {
            Serial.println("Verbunden!");
            
            // Subscribe zu brightness Topic (aus old.cpp)
            String sharedTopic = projectPrefix + "/brightness";
            mqttClient.subscribe(sharedTopic.c_str());
            Serial.println("📡 Subscribed to: " + sharedTopic);
            
        } else {
            Serial.print("Fehler: ");
            Serial.println(mqttClient.state());
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

// WiFi Reconnect aus old.cpp adaptiert
void WifiMQTTManager::reconnectWiFi() {
    WiFi.disconnect();
    vTaskDelay(pdMS_TO_TICKS(1000));
    WiFi.begin(); // Versucht gespeicherte Credentials
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
        attempts++;
    }
}

void WifiMQTTManager::handleIncomingData() {
    // MQTT Loop für eingehende Nachrichten (aus old.cpp)
    if (!mqttClient.connected()) {
        connectToMQTT();
    }
    mqttClient.loop();
}

void WifiMQTTManager::sendSensorData() {
    if (pressureSensor != nullptr) {
        bool currentSensorState = pressureSensor->isPressed();
        
        // Nur senden bei Änderung
        if (currentSensorState != lastSensorState) {
            lastSensorState = currentSensorState;
            
            // MQTT Publish für Sensor (adaptiert aus old.cpp Button-Logic)
            String msg = espnumber + " " + (currentSensorState ? "1" : "0");
            String topic = projectPrefix + "/sensors";
            
            mqttClient.publish(topic.c_str(), msg.c_str());
            Serial.printf("📤 Sensor MQTT: %s (Topic: %s)\n", msg.c_str(), topic.c_str());
        }
    }
}

void WifiMQTTManager::sendButtonData() {
    if (chairState != nullptr) {
        int currentChairState = chairState->getCurrentState();
        
        // Nur senden bei Änderung
        if (currentChairState != lastChairState) {
            lastChairState = currentChairState;
            
            // MQTT Publish für ChairState (neue Funktionalität)
            String msg = espnumber + " " + String(currentChairState);
            String topic = projectPrefix + "/buttons";  // Verwende buttons topic wie in old.cpp
            
            mqttClient.publish(topic.c_str(), msg.c_str());
            Serial.printf("📤 ChairState MQTT: %s (%s) (Topic: %s)\n", 
                         msg.c_str(), chairState->getCurrentStateName(), topic.c_str());
        }
    }
}

bool WifiMQTTManager::getConnectionStatus() const {
    return isConnected;
}
