#ifndef __MDNS_H__
#define __MDNS_H__

#include <ESP8266mDNS.h>
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
            MDNS.enableArduino(OTA_PORT, OTA_AUTH);
            MDNS.addService("websock", "tcp", SRV_WS_PORT);
            MDNS.addService("message", "tcp", SRV_MESSAGE_PORT);
            MDNS.addService("http", "tcp", SRV_WEB_PORT);
            MDNS.addService("remotecontrol", "udp", SRV_UDP_PORT);
            MDNS.update();
            mdnsInitialized = true;
        }
    }
    if (mdnsInitialized) MDNS.update();
}

void mdns_update() {
    MDNS.notifyAPChange();
    MDNS.update();
}

#endif // __MDNS_H__