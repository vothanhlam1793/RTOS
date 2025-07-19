#include <Arduino.h>

#include "TaskBlink.h"
#include "TaskButtonInput.h"
#include "TaskCLI.h"  // 🆕
#include "TaskIRReceive.h"
#include "TaskIRSender.h"
#include "TaskLogger.h"
#include "TaskNetTool.h"
#include "TaskWiFi.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    while (!Serial) {
    }
    Serial.println("He thong bat dau .... ");
    TaskWiFi_Start();
    TaskCLI_Start();  // 🆕
    TaskBlink_Start();
    TaskNetTool_Start();
    // TaskLogger_Start();
    TaskIRReceive_Start();
    // TaskIRSender_Start();
    // TaskButtonInput_Start();
}

void loop() {
    // put your main code here, to run repeatedly:
    TaskCLI_Handle();  // 🆕 đọc & xử lý lệnh CLI
}
