#include "TaskLogger.h"

#include <Arduino.h>

void TaskLogger(void *pvParameters) {
    while (1) {
        Serial.println("Logger task is alive!");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void TaskLogger_Start() { xTaskCreate(TaskLogger, "Logger", 2048, NULL, 1, NULL); }
