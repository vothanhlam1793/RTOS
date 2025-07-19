#include "TaskWiFi.h"

#include <Preferences.h>
#include <WiFi.h>

#include "TaskCLI.h"

Preferences pref;

void connectWiFi(const char* ssid, const char* pass) {
    WiFi.begin(ssid, pass);
    Serial.printf("🔌 Kết nối tới SSID: %s...\n", ssid);
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry++ < 20) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n✅ Đã kết nối! IP: %s\n", WiFi.localIP().toString().c_str());
    } else {
        Serial.println("\n❌ Kết nối thất bại.");
    }
}

static void wifiCli(const std::vector<String>& args) {
    if (args.empty()) {
        Serial.println("wifi scan | set <ssid> <pass>");
        return;
    }

    if (args[0] == "scan") {
        // gọi hàm quét Wi-Fi
        wifi_scan();
    } else if (args[0] == "set" && args.size() == 3) {
        String ssid = args[1];
        String pass = args[2];
        // gọi hàm lưu cấu hình & connect Wi-Fi
        wifi_set(ssid.c_str(), pass.c_str());
    } else {
        Serial.println("❌ Cú pháp: wifi scan | wifi set <ssid> <pass>");
    }
}

void TaskWiFi_Start() {
    CLI_RegisterCommand("wifi", "wifi scan | wifi set <ssid> <pass>", wifiCli);
    pref.begin("wifi", true);  // readonly
    String ssid = pref.getString("ssid", "");
    String pass = pref.getString("pass", "");
    pref.end();

    if (ssid.length() > 0) {
        connectWiFi(ssid.c_str(), pass.c_str());
    } else {
        Serial.println("⚠️  Chưa cấu hình WiFi. Dùng CLI: wifi scan / wifi set");
    }
}

void wifi_scan() {
    Serial.println("🔍 Quét WiFi xung quanh...");
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("❌ Không tìm thấy mạng nào.");
    } else {
        for (int i = 0; i < n; ++i) {
            Serial.printf("%d: %s (RSSI: %d)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
        }
    }
}

void wifi_set(const char* ssid, const char* pass) {
    pref.begin("wifi", false);
    pref.putString("ssid", ssid);
    pref.putString("pass", pass);
    pref.end();
    Serial.printf("💾 Đã lưu WiFi: %s\n", ssid);
    connectWiFi(ssid, pass);
}
