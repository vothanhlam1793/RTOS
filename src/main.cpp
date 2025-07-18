#include <Arduino.h>

#include "TaskBlink.h"
#include "TaskButtonInput.h"
#include "TaskIRReceive.h"
#include "TaskIRSender.h"
#include "TaskLogger.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    while (!Serial) {
    }
    Serial.println("He thong bat dau .... ");
    TaskBlink_Start();
    // TaskLogger_Start();
    TaskIRReceive_Start();
    // TaskIRSender_Start();
    // TaskButtonInput_Start();
}

void loop() {
    // put your main code here, to run repeatedly:
}
