#include "TaskBlink.h"

#include <Arduino.h>
#include <Preferences.h>

#include "TaskCLI.h"

Preferences pref_blink;
volatile uint8_t pinB;
volatile uint32_t freq;
volatile bool ena;

void blink_reload();
void blink_set(uint8_t pin, uint32_t fre);
void blink_control(const String& ctrl);

/* ---------- CLI ---------- */
static void blinkCli(const std::vector<String>& a) {
    if (a.empty()) {
        Serial.println("blink enable <on/off>   - bật / tắt nháy");
        Serial.println("blink set <pin> <freq>  - chọn chân & tần số (Hz)");
        Serial.println("blink list              - danh sách pin khả dụng");
        return;
    }

    if (a[0] == "list") {
        Serial.println("Pin 2");
    } else if (a[0] == "set" && a.size() == 3) {
        blink_set(a[1].toInt(), a[2].toInt());
    } else if (a[0] == "enable" && a.size() == 2) {
        blink_control(a[1]);
    } else {
        Serial.println("❓ Sai cú pháp – gõ 'blink' để xem hướng dẫn.");
    }
}

/* ---------- FreeRTOS task ---------- */
void TaskBlink(void*) {
    blink_reload();
    pinMode(pinB, OUTPUT);

    for (;;) {
        if (ena && freq) {
            digitalWrite(pinB, HIGH);
            vTaskDelay(pdMS_TO_TICKS(500 / freq));  // nửa chu kỳ
            digitalWrite(pinB, LOW);
            vTaskDelay(pdMS_TO_TICKS(500 / freq));
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));  // tránh busy loop
        }
    }
}

void TaskBlink_Start() {
    CLI_RegisterCommand("blink", "blink enable <on/off> | blink set <pin> <freq> | blink list", blinkCli);
    xTaskCreate(TaskBlink, "Blink", 4096, nullptr, 1, nullptr);
}

/* ---------- Helpers ---------- */
void blink_set(uint8_t pin, uint32_t fre) {
    Serial.printf("🔧 PIN=%d  FREQ=%d Hz\n", pin, fre);
    pref_blink.begin("blink", false);
    pref_blink.putInt("pin", pin);
    pref_blink.putUInt("freq", fre);
    pref_blink.end();
    blink_reload();
}

void blink_control(const String& c) {
    bool newState = (c == "on");
    Serial.printf("Cài đặt trạng thái mới: %s\n", newState ? "ON" : "OFF");
    pref_blink.begin("blink", false);
    pref_blink.putBool("enable", newState);
    pref_blink.end();
    blink_reload();
}

void blink_reload() {
    pref_blink.begin("blink", true);
    pinB = pref_blink.getInt("pin", 2);
    freq = pref_blink.getUInt("freq", 1);
    ena = pref_blink.getBool("enable", true);
    pref_blink.end();
}
