#include "TaskIRReceive.h"

#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

// Cấu hình chân IR Receiver
const uint16_t kRecvPin = 15;
IRrecv irrecv(kRecvPin);
decode_results results;

void TaskIRReceive(void *pvParameters) {
    irrecv.enableIRIn();    // Bắt đầu nhận tín hiệu
    uint32_t ir_count = 0;  // Biến đếm số lần nhận được
    while (1) {
        if (irrecv.decode(&results)) {
            // Hiển thị thông tin IR nhận được
            ir_count++;
            Serial.print(ir_count);
            Serial.println(" - Nhận:");
            Serial.println(resultToHumanReadableBasic(&results));
            // Serial.printf("%d: Mã HEX: 0x%08X\n", ir_count, results.value);
            if (results.decode_type == UNKNOWN) {
                Serial.print("rawdata: [");
                for (uint16_t i = 1; i < results.rawlen; i++) {
                    Serial.print(results.rawbuf[i] * kRawTick);  // kRawTick = 50
                    if (i < results.rawlen - 1) Serial.print(", ");
                }
                Serial.println("]");
                Serial.printf("rawlen: %d, frequency: 38 Hz\n", results.rawlen);
            }
            irrecv.resume();  // Chuẩn bị nhận tiếp
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);  // Delay nhỏ tránh chiếm CPU
    }
}

void TaskIRReceive_Start() { xTaskCreate(TaskIRReceive, "IRReceive", 4096, NULL, 1, NULL); }
