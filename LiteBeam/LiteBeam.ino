/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/////////////////////// Features ///////////////////////


/////////////////////// User defined settings ///////////////////////
#define OTA_PASSWORD "GrblOTA"              // ESP OTA Update password (Comment to disable)
//#define OTA_PORT 8266                     // Custom ESP OTA port

//#define DEFAULT_ST_SSID "PrettyFly"       // Your network ssid. 
//#define DEAFULT_ST_PASSWORD "ForAWiFi"    // Your network password (if any)

//#define DEFAULT_AP_SSID "LiteBeam"        // Default AP network SSID
//#define DEFAULT_AP_PASSWORD ""            // Default AP network password (empty for open WiFi network)

//#define DEFAULT_HOSTNAME "LiteBeam"       // Default hostname for your device
//#define WIFI_DEBUG                        // Send serial messages to find out what WiFi network is hosted/being connected to 

//#define WS_DEBUG                          // Prints a "connected X" message on Serial when a WebSocket connection is created

#define RESET_PIN 2                         // GPIO2 on ESP01; Connect to arduino reset pin



#include <Arduino.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <WiFiUdp.h>
#include <math.h>
#include <pgmspace.h>

#include "favicon.h"
#include "resources.h"
#include "settings.h"

#include "liteWeb.h"
#include "liteMsg.h"
#include "liteWs.h"         
#include "liteWifi.h"
#include "liteUdp.h"         
#include "liteMdns.h"       
#include "liteSerial.h"  

#include "stk500impl.h"



#define BUF_SIZE 512
byte dataBuffer[BUF_SIZE];

void setup() {
    init_eeprom();
    init_serial(dataBuffer, BUF_SIZE, handle_serial);
    init_wifi();
    
#ifdef OTA_PASSWORD
    ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
#ifdef OTA_PORT
    ArduinoOTA.setPort(OTA_PORT);
#endif
    ArduinoOTA.begin(false);
    
    reset_arduino();

    stk500_init(reset_arduino);

    init_web(reset_arduino);
    init_msg(dataBuffer, BUF_SIZE);
    init_ws();
    init_udp(dataBuffer, BUF_SIZE);
}

void loop() {
    ArduinoOTA.handle();

    wifi_loop();
    mdns_loop();
    ws_loop();
    udp_loop();
    msg_loop();
    serial_loop();
    web_loop(wifi_is_captive());
}

void handle_serial(byte* buff, unsigned int n) {
    udp_send(buff, n);
    msg_send(buff, n);
    ws_send(buff, n);
}

void reset_arduino() {
    pinMode(RESET_PIN, OUTPUT_OPEN_DRAIN);
    digitalWrite(RESET_PIN, LOW);
    delay(10);
    digitalWrite(RESET_PIN, HIGH);
}

