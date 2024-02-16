
#include <ESP8266WiFi.h>
#include "settings.h"
#include "liteWifi.h"

void init_wifi() {
    SettingsStruct *s = get_settings();
    WiFi.mode(WIFI_AP_STA);
    wifi_change_ap();
    wifi_change_st();
    if (strlen(s->hostname)) {
        WiFi.setHostname(s->hostname);
    }
}

void wifi_change_ap() {
    SettingsStruct *s = get_settings();
    WiFi.softAPdisconnect();
    if (strlen(s->apssid)) {
        if (strlen(s->appswd) <= 8)
            WiFi.softAP(s->apssid);
        else
            WiFi.softAP(s->apssid, s->appswd);
    }
}

void wifi_change_st(){
    SettingsStruct *s = get_settings();
    if (WiFi.isConnected())
        WiFi.disconnect();
    if (strlen(s->ssid)) {
        if (strlen(s->pswd))
            WiFi.begin(s->ssid);
        else
            WiFi.begin(s->ssid, s->pswd);
    }
}

