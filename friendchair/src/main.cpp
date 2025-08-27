#include <Arduino.h>
#include "state/ChairState.h"
#include "sensor/PressureSensor.h"
#include "input/StateButton.h"
#include "tasks/TaskManager.h"
#include "network/NetworkManager.h"
#include "output/OutputManager.h"


// #########################################
const String ESP_NUMBER = "01";        // ESP-Nummer (zweistellig)
// #########################################

const int LED_PIN = 26;
const int SENSOR_PIN = 33;
const int SENSOR_THRESHOLD = 3000;
const int BUTTON_PIN = 14;
const int BUILTIN_LED_PIN = 2;         // Built-in LED Pin



ChairState chairState;
PressureSensor pressureSensor(SENSOR_PIN, SENSOR_THRESHOLD);
StateButton stateButton(BUTTON_PIN, &chairState);
TaskManager taskManager;
WifiMQTTManager networkManager;
OutputManager outputManager;

void setup() {
    Serial.begin(115200);
    
    outputManager.init(LED_PIN, BUILTIN_LED_PIN);
    pressureSensor.init();
    stateButton.init();
    
    taskManager.init(&chairState, &pressureSensor, &stateButton);
    networkManager.init(&chairState, &pressureSensor, &stateButton, &outputManager, ESP_NUMBER, BUILTIN_LED_PIN);
    
    Serial.println("Friend's Chair setup complete.");
    
    taskManager.startTasks();
    networkManager.startTasks();
    
    Serial.println("All parallel tasks running...");
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000)); // CPU für andere Tasks freigeben - spart Energie und verhindert Blockierung
}