#pragma once
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "CubeWifi";
const char *password = "mycoolcubepassword";

IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void communication_setup()
{
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(gateway, gateway, subnet);
}

bool communication_wait_for_client()
{
    return WiFi.softAPgetStationNum() != 0;
}

void communication_send_message(uint32_t colors[5])
{
    char buf[512];
    snprintf(buf, 512, "%u\n%u\n%u\n%u\n%u\n", colors[0], colors[1], colors[2], colors[3], colors[4]);
    HTTPClient http;
    http.begin("http://192.168.1.184/");
    int code = http.GET();
    Serial.println(code);
    String message = http.getString();
    http.end();
}