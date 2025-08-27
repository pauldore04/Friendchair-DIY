#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "state/ChairState.h"
#include "sensor/PressureSensor.h"
#include "input/StateButton.h"

class TaskManager {
private:
    static ChairState* chairState;
    static PressureSensor* pressureSensor;
    static StateButton* stateButton;
    
    static void buttonTask(void* parameter);
    static void sensorTask(void* parameter);
    
public:
    TaskManager();
    void init(ChairState* state, PressureSensor* sensor, StateButton* button);
    void startTasks();
};

#endif
