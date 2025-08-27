#include "tasks/TaskManager.h"
#include <Arduino.h>

ChairState* TaskManager::chairState = nullptr;
PressureSensor* TaskManager::pressureSensor = nullptr;
StateButton* TaskManager::stateButton = nullptr;

TaskManager::TaskManager() {
}

void TaskManager::init(ChairState* state, PressureSensor* sensor, StateButton* button) {
    chairState = state;
    pressureSensor = sensor;
    stateButton = button;
}

void TaskManager::buttonTask(void* parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(50); // 50ms delay
    
    while (true) {
        if (stateButton != nullptr) {
            stateButton->update();
        }
        vTaskDelay(xDelay);
    }
}

void TaskManager::sensorTask(void* parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(100); // 100ms delay
    
    while (true) {
        if (pressureSensor != nullptr) {
            pressureSensor->update();
        }
        vTaskDelay(xDelay);
    }
}

void TaskManager::startTasks() {

    xTaskCreate(
        buttonTask,     // Task function
        "ButtonTask",   // Task name
        2048,          // Stack size
        NULL,          // Parameters
        2,             // Priority (higher than sensor)
        NULL           // Task handle
    );
    

    xTaskCreate(
        sensorTask,     // Task function
        "SensorTask",   // Task name
        2048,          // Stack size
        NULL,          // Parameters
        1,             // Priority (lower than button)
        NULL           // Task handle
    );
    
    Serial.println("FreeRTOS tasks started:");
    Serial.println("- ButtonTask (Priority 2, 50ms cycle)");
    Serial.println("- SensorTask (Priority 1, 100ms cycle)");
}
