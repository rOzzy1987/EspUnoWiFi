#ifndef __MDNS_H__
#define __MDNS_H__

#include <Esp.h>

#ifdef ESP32
#include <ESPmDNS.h>
#include <Wifi.h>
#else // ESP8266
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#endif


#include "config.h"
#include "settings.h"
#include "liteMdns.h"


#ifdef OTA_PASSWORD
#define OTA_AUTH true
#else 
#define OTA_AUTH false
#endif

#ifndef OTA_PORT
#define OTA_PORT 8266
#endif

static bool mdnsInitialized = false;

void mdns_loop() {
    if (!mdnsInitialized && WiFi.status() == WL_CONNECTED) {
        if (MDNS.begin(get_settings()->hostname)) {
            MDNS.addService("http", "tcp", SRV_WEB_PORT);
#ifdef FEATURE_OTA
            MDNS.enableArduino(OTA_PORT, OTA_AUTH);
#endif
#ifdef FEATURE_WS
            MDNS.addService("websock", "tcp", SRV_WS_PORT);
#endif
#ifdef FEATURE_TCP
            MDNS.addService("message", "tcp", SRV_TCP_PORT);
#endif
#ifdef FEATURE_UDP
            MDNS.addService("remotecontrol", "udp", SRV_UDP_PORT);
#endif
#ifndef ESP32
            MDNS.update();
#endif
            mdnsInitialized = true;
        }
    }
#ifndef ESP32
    if (mdnsInitialized) 
        MDNS.update();
#endif
}

void mdns_update() {
#ifndef ESP32
    MDNS.notifyAPChange();
    MDNS.update();
#endif
}

#endif // __MDNS_H__