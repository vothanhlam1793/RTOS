#include "TaskIRSender.h"

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define IR_SEND_PIN 14  // Bạn chọn chân nào tùy

IRsend irsend(IR_SEND_PIN);

// ---------------------
// Dữ liệu demo (3 nút)
// ---------------------

// rawdata cho lệnh 11
const uint16_t rawData_11[80] = {528, 1640, 512, 544,  530, 544,  554, 1616, 536, 1612, 536, 520,  552, 1616, 534, 522,
                                 550, 524,  550, 524,  550, 1618, 532, 1618, 532, 1618, 532, 1618, 532, 1618, 532, 1618,
                                 532, 1618, 532, 1618, 532, 524,  548, 526,  548, 526,  550, 526,  548, 526,  548, 526,
                                 548, 1620, 530, 526,  548, 1620, 530, 1620, 530, 526,  548, 528,  548, 526,  546, 1624,
                                 528, 548,  524, 1626, 524, 552,  524, 550,  522, 1628, 522, 1628, 522, 1628, 520};

// rawdata cho lệnh 13
const uint16_t rawData_13[52] = {530,  1620, 528,  1620, 528,  528, 546,  528, 548,  528, 546,  528, 546,
                                 1622, 526,  550,  524,  552,  524, 1624, 524, 1628, 522, 1628, 520, 554,
                                 520,  554,  520,  556,  518,  556, 520,  554, 518,  556, 518,  556, 520,
                                 556,  518,  1632, 518,  1632, 518, 1632, 518, 1632, 518, 1632, 518};

// rawdata cho lệnh 15
const uint16_t rawData_15[73] = {1618, 530, 1620, 510, 546,  528, 1640, 510, 544,  530, 546,  530, 546,  550, 1618, 536,
                                 1614, 534, 1616, 532, 1616, 532, 1616, 532, 1616, 532, 1618, 530, 1620, 530, 526,  548,
                                 530,  544, 526,  548, 526,  548, 526,  548, 526,  548, 1620, 534, 524,  548, 1620, 530,
                                 1620, 530, 526,  548, 526,  548, 528,  548, 1620, 528, 526,  548, 1622, 528, 528,  546,
                                 528,  546, 1622, 526, 1622, 526, 1624, 524};

void sendButton(uint8_t buttonId) {
    switch (buttonId) {
        case 1:
            irsend.sendCOOLIX(0xB21F58, 24);
            delay(100);
            irsend.sendRaw(rawData_11, 80, 38);
            break;
        case 2:
            irsend.sendCOOLIX(0xB27BE0, 24);
            delay(100);
            irsend.sendRaw(rawData_13, 52, 38);
            break;
        case 3:
            irsend.sendCOOLIX(0xB21F58, 24);
            delay(100);
            irsend.sendRaw(rawData_15, 73, 38);
            break;
        default:
            Serial.println("❌ Nút không hợp lệ");
    }
}

void TaskIRSender(void *pvParameters) {
    irsend.begin();
    Serial.println("🚀 IR Sender Ready!");
    while (1) {
        // Có thể đợi lệnh từ queue ở đây trong tương lai
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void TaskIRSender_Start() { xTaskCreate(TaskIRSender, "IRSender", 4096, NULL, 1, NULL); }

void IRSendButton(uint8_t buttonId) { sendButton(buttonId); }
