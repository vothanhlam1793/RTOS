#include "TaskCLI.h"

#include <Arduino.h>
#include <WiFi.h>

#include <map>
#include <vector>

namespace {
// ===== Cấu hình =====
constexpr size_t BUF_SIZE = 128;  // Độ dài tối đa của chuỗi lệnh
char cmdBuf[BUF_SIZE];
size_t cmdLen = 0;

struct CmdEntry {
    const char* help;
    CliCallback cb;
};

std::map<String, CmdEntry> registry;  // Bảng lệnh đã đăng ký

// ---- Tách chuỗi thành token, hỗ trợ "SSID có khoảng trắng" ----
std::vector<String> splitArgs(const String& line) {
    std::vector<String> out;
    String tok;
    bool inQuotes = false;
    char quoteChar = 0;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if ((c == '"' || c == '\'') && !inQuotes) {  // bắt đầu " hoặc '
            inQuotes = true;
            quoteChar = c;
        } else if (c == quoteChar && inQuotes) {  // kết thúc chuỗi trích dẫn
            inQuotes = false;
            out.push_back(tok);
            tok = "";
        } else if (c == ' ' && !inQuotes) {  // khoảng trắng ngoài dấu "…"
            if (tok.length() > 0) {
                out.push_back(tok);
                tok = "";
            }
        } else {
            tok += c;
        }
    }
    if (tok.length() > 0) out.push_back(tok);
    return out;
}

// ---- In bảng trợ giúp ----
void showHelp() {
    Serial.println(F("\n===== CLI commands ====="));
    for (auto& kv : registry) {
        Serial.printf("%-12s - %s\n", kv.first.c_str(), kv.second.help);
    }
    Serial.println(F("========================\n"));
}

// ---- Phân phối lệnh ----
void dispatch(const String& line) {
    auto tokens = splitArgs(line);
    if (tokens.empty()) return;

    String cmd = tokens[0];
    if (cmd == "help") {
        showHelp();
        return;
    }

    auto it = registry.find(cmd);
    if (it == registry.end()) {
        Serial.println(F("❓ Lệnh không có. Gõ 'help' để xem danh sách."));
        return;
    }
    std::vector<String> args(tokens.begin() + 1, tokens.end());
    it->second.cb(args);  // Gọi callback của task
}

}  // namespace

// ===== API công khai =====
bool CLI_RegisterCommand(const char* name, const char* help, CliCallback cb) {
    if (!name || !cb) return false;
    registry[String(name)] = {help, cb};
    return true;
}

void TaskCLI_Start() {
    // Lệnh hệ thống mặc định
    CLI_RegisterCommand("reboot", "Khởi động lại thiết bị", [](const std::vector<String>&) {
        Serial.println(F("♻️  Rebooting..."));
        delay(100);
        ESP.restart();
    });

    Serial.println(F("🖥️  CLI sẵn sàng – gõ 'help' để xem lệnh."));
}

// void TaskCLI_Handle() {
//     while (Serial.available()) {
//         char c = Serial.read();
//         if (c == '\r' || c == '\n') {
//             if (cmdLen) {
//                 cmdBuf[cmdLen] = '\0';
//                 dispatch(String(cmdBuf));
//                 cmdLen = 0;
//             }
//         } else if (cmdLen < BUF_SIZE - 1) {
//             cmdBuf[cmdLen++] = c;
//         }
//     }
// }

void TaskCLI_Handle() {
    while (Serial.available()) {
        char c = Serial.read();

        // ---- Xử lý xuống dòng ----
        if (c == '\r' || c == '\n') {
            Serial.println();  // echo newline
            if (cmdLen) {
                cmdBuf[cmdLen] = 0;
                dispatch(String(cmdBuf));
                cmdLen = 0;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
            Serial.print("> ");  // prompt lại
            continue;
        }

        // ---- Xử lý Backspace ----
        if (c == 0x08 || c == 0x7F) {  // BS hoặc DEL
            if (cmdLen > 0) {
                cmdLen--;
                Serial.print("\b \b");  // xóa trên terminal
            }
            continue;
        }

        // ---- Lưu & echo ký tự bình thường ----
        if (cmdLen < BUF_SIZE - 1) {
            cmdBuf[cmdLen++] = c;
            Serial.write(c);  // echo lại
        }
    }
}
