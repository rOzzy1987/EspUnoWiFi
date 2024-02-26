#include <Esp.h>

#ifdef ESP32
#include <Wifi.h>
static wifi_mode_t mode;
#else // ESP8266
#include <ESP8266WiFi.h>
static WiFiMode mode;
#endif

#include <DNSServer.h>
#include "config.h"
#include "settings.h"
#include "liteWifi.h"


static DNSServer* dns;

static IPAddress captiveIp(192,168,4,1);
static IPAddress subnetMask(255,255,255,0);

void wifi_init_ap() {
    SettingsStruct *s = get_settings();
    mode = WIFI_AP;
    WiFi.mode(mode);

    if (strlen(s->apssid)) {

        if (dns == NULL){
            dns = new DNSServer();
        }
        dns->start(SRV_DNS_PORT, "*", captiveIp);

        WiFi.softAPConfig(captiveIp, captiveIp, subnetMask);

        if (strlen(s->appswd) < 8)
            WiFi.softAP(s->apssid);
        else
            WiFi.softAP(s->apssid, s->appswd);

#ifdef WIFI_DEBUG
        Serial.printf("AP ready: %s\n", s->apssid);
#endif
    } else {
        load_eeprom(true);
    }
}

void wifi_init_st(){
    if (mode != WIFI_STA) {
        wifi_init_ap();
        return;
    }

    SettingsStruct *s = get_settings();
    WiFi.mode(mode);
    if (strlen(s->ssid)) {
        if (strlen(s->pswd) < 8) {
            WiFi.begin(s->ssid);
        } else {
            WiFi.begin(s->ssid, s->pswd);
        }
#ifdef WIFI_DEBUG
        Serial.printf("Connecting to: %s\n", s->ssid);
#endif

        if(WiFi.waitForConnectResult() == WL_CONNECTED) {
#ifdef WIFI_DEBUG
            Serial.println(WiFi.localIP().toString().c_str());
#endif
            return;
        }
    } 
#ifdef WIFI_DEBUG
    Serial.printf("AP fallback\n");
#endif
    wifi_init_ap();
}

bool wifi_is_captive() {
    return mode == WIFI_AP;
}

void init_wifi() {
    SettingsStruct *s = get_settings();
    mode = (strlen(s->ssid) > 0) ? WIFI_STA : WIFI_AP;
    if (strlen(s->hostname)) {
        WiFi.setHostname(s->hostname);
    }

    wifi_init_st();
}

void wifi_loop() {
    if (dns == NULL) return;
    dns->processNextRequest();
}



