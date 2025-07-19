#include "TaskNetTool.h"

#include <Arduino.h>
#include <ESP32Ping.h>

#include "TaskCLI.h"

static void netCli(const std::vector<String>& args) {
    if (args.empty()) {
        Serial.println("net ping <host>     - Kiểm tra kết nối (5 gói)");
        return;
    }

    if (args[0] == "ping" && args.size() == 2) {
        String host = args[1];
        IPAddress ip;

        if (!WiFi.hostByName(host.c_str(), ip)) {
            Serial.println("❌ Không phân giải được hostname.");
            return;
        }

        Serial.printf("🔍 Ping %s (%s): 5 gói, 32 byte dữ liệu\n", host.c_str(), ip.toString().c_str());

        const uint8_t COUNT = 5;
        uint32_t rttSum = 0, rttMin = UINT32_MAX, rttMax = 0;
        uint8_t lost = 0;

        for (uint8_t i = 1; i <= COUNT; ++i) {
            int32_t rtt = Ping.ping(ip, 1) ? Ping.averageTime() : -1;

            if (rtt >= 0) {
                Serial.printf("  %d bytes từ %s: icmp_seq=%d time=%ld ms\n", 32, ip.toString().c_str(), i, rtt);
                rttSum += rtt;
                rttMin = min<uint32_t>(rttMin, rtt);
                rttMax = max<uint32_t>(rttMax, rtt);
            } else {
                Serial.printf("  icmp_seq=%d **lost**\n", i);
                lost++;
            }
            delay(1000);  // 1 s giữa các gói
        }

        uint8_t recv = COUNT - lost;
        Serial.printf("\n--- %s ping statistics ---\n", host.c_str());
        Serial.printf("%d packets transmitted, %d received, %d%% packet loss\n", COUNT, recv, (lost * 100) / COUNT);

        if (recv) {
            Serial.printf("rtt min/avg/max = %lu/%.1f/%lu ms\n", rttMin, (float)rttSum / recv, rttMax);
        }
    } else {
        Serial.println("❓ Sai cú pháp – gõ 'net' để xem hướng dẫn.");
    }
}

void TaskNetTool_Start() { CLI_RegisterCommand("net", "net ping <host>", netCli); }
