#include "TaskButtonInput.h"

#include <Arduino.h>

#include "TaskIRSender.h"

// Định nghĩa chân nút
#define BTN1_PIN 25
#define BTN2_PIN 26
#define BTN3_PIN 27

void TaskButtonInput(void *pvParameters) {
    // Cấu hình chân nút
    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT_PULLUP);

    bool prev1 = HIGH, prev2 = HIGH, prev3 = HIGH;

    while (1) {
        bool b1 = digitalRead(BTN1_PIN);
        bool b2 = digitalRead(BTN2_PIN);
        bool b3 = digitalRead(BTN3_PIN);

        // Phát hiện nút nhấn xuống (LOW) và chống rung đơn giản
        if (b1 == LOW && prev1 == HIGH) {
            Serial.println("🔘 Button 1 nhấn");
            IRSendButton(1);
        }
        if (b2 == LOW && prev2 == HIGH) {
            Serial.println("🔘 Button 2 nhấn");
            IRSendButton(2);
        }
        if (b3 == LOW && prev3 == HIGH) {
            Serial.println("🔘 Button 3 nhấn");
            IRSendButton(3);
        }

        prev1 = b1;
        prev2 = b2;
        prev3 = b3;

        vTaskDelay(50 / portTICK_PERIOD_MS);  // chống rung và giảm CPU
    }
}

void TaskButtonInput_Start() { xTaskCreate(TaskButtonInput, "ButtonInput", 2048, NULL, 1, NULL); }
