#include "TaskBlink.h"

#include <Arduino.h>

void TaskBlink(void *pvParameters) {
    pinMode(LED_BUILTIN, OUTPUT);
    while (1) {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void TaskBlink_Start() { xTaskCreate(TaskBlink, "Blink", 1024, NULL, 1, NULL); }
