// ===== include/TaskCLI.h =====
#ifndef TASK_CLI_H
#define TASK_CLI_H

#include <Arduino.h>

#include <vector>

// Kiểu hàm callback cho 1 lệnh: nhận args đã cắt sẵn
using CliCallback = void (*)(const std::vector<String>& args);

// Đăng ký 1 lệnh mới
//   name  : từ khoá lệnh (không có dấu cách, vd: "wifi")
//   help  : mô tả ngắn, hiển thị khi gõ 'help'
//   cb    : hàm xử lý
bool CLI_RegisterCommand(const char* name, const char* help, CliCallback cb);

// Hàm khởi động & vòng lặp CLI
void TaskCLI_Start();
void TaskCLI_Handle();

#endif
