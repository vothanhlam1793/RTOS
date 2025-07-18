#ifndef TASK_IR_SENDER_H
#define TASK_IR_SENDER_H

#include <stdint.h>

void TaskIRSender_Start();
void IRSendButton(uint8_t buttonId);  // Gửi lệnh tương ứng nút 1–3

#endif
