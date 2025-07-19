#include "TaskCLI.h"

#include <Arduino.h>
#include <WiFi.h>

#include <cstring>  // for memcmp
#include <map>
#include <vector>

constexpr size_t CLI_BUF_SIZE = 128;
constexpr size_t HISTORY_DEPTH = 5;

char cmdBuf[CLI_BUF_SIZE];
size_t cmdLen = 0;

char history[HISTORY_DEPTH][CLI_BUF_SIZE];
int historyCount = 0;
int historyIndex = -1;

bool escSeq = false;
char escBuf[3];
int escIdx = 0;

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

// }  // namespace

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
void TaskCLI_Handle() {
    while (Serial.available()) {
        char c = Serial.read();

        // --- Đọc chuỗi ESC ---
        if (escSeq) {
            escBuf[escIdx++] = c;
            if (escIdx == 2) {
                if (escBuf[0] == '[') {
                    if (escBuf[1] == 'A') {
                        // ↑ recall lệnh cũ
                        if (historyCount > 0 && historyIndex > 0) {
                            historyIndex--;
                            strncpy(cmdBuf, history[historyIndex % HISTORY_DEPTH], CLI_BUF_SIZE);
                            cmdLen = strlen(cmdBuf);
                            Serial.print("\r> ");
                            Serial.print(cmdBuf);
                            Serial.print(" \b");
                        }
                    } else if (escBuf[1] == 'B') {
                        // ↓ recall lệnh mới
                        if (historyCount > 0 && historyIndex < historyCount - 1) {
                            historyIndex++;
                            strncpy(cmdBuf, history[historyIndex % HISTORY_DEPTH], CLI_BUF_SIZE);
                            cmdLen = strlen(cmdBuf);
                            Serial.print("\r> ");
                            Serial.print(cmdBuf);
                            Serial.print(" \b");
                        } else {
                            historyIndex = historyCount;
                            cmdLen = 0;
                            cmdBuf[0] = '\0';
                            Serial.print("\r> ");
                        }
                    }
                }
                escSeq = false;
                escIdx = 0;
            }
            continue;
        }

        if (c == 0x1B) {  // ESC
            escSeq = true;
            escIdx = 0;
            continue;
        }

        // --- Xuống dòng ---
        if (c == '\r' || c == '\n') {
            Serial.println();
            if (cmdLen > 0) {
                cmdBuf[cmdLen] = '\0';
                dispatch(String(cmdBuf));
                strncpy(history[historyCount % HISTORY_DEPTH], cmdBuf, CLI_BUF_SIZE);
                historyCount++;
                historyIndex = historyCount;
                cmdLen = 0;
            }
            Serial.print("> ");
            continue;
        }

        // --- Backspace ---
        if (c == 0x08 || c == 0x7F) {
            if (cmdLen > 0) {
                cmdLen--;
                Serial.print("\b \b");
            }
            continue;
        }

        // --- Thêm ký tự hợp lệ ---
        if (cmdLen < CLI_BUF_SIZE - 1) {
            if (c >= 32 && c <= 126) {  // ASCII printable
                cmdBuf[cmdLen++] = c;
                Serial.write(c);
            } else {
                // Uncomment để debug ký tự lạ:
                // Serial.printf("\n⚠️ Bỏ qua ký tự không hợp lệ: 0x%02X\n> %s", (uint8_t)c, cmdBuf);
            }
        }
    }
}
