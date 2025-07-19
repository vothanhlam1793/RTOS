#ifndef TASK_WIFI_H
#define TASK_WIFI_H

void TaskWiFi_Start();
void wifi_scan();                                   // dùng từ CLI
void wifi_set(const char* ssid, const char* pass);  // dùng từ CLI

#endif
