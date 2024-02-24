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

// Edit config.h to customize your firmware
#include "config.h"

#include <Arduino.h>

#ifdef FEATURE_OTA
#include <ArduinoOTA.h>
#endif

#include "favicon.h"
#include "resources.h"
#include "settings.h"

#include "liteWeb.h"
#include "liteWifi.h"   
#include "liteMdns.h"       
#include "liteSerial.h"  

#ifdef FEATURE_WS
#include "liteWs.h"
#endif         

#ifdef FEATURE_TCP
#include "liteTcp.h"
#endif

#ifdef FEATURE_UDP
#include "liteUdp.h"
#endif      

#ifdef FEATURE_STK500
#include "stk500impl.h"
#endif



#define BUF_SIZE 512
byte dataBuffer[BUF_SIZE];

void setup() {
    init_eeprom();
    init_serial(dataBuffer, BUF_SIZE, handle_serial);
    init_wifi();

#ifdef FEATURE_OTA
#ifdef OTA_PASSWORD
    ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
#ifdef OTA_PORT
    ArduinoOTA.setPort(OTA_PORT);
#endif
    ArduinoOTA.begin(false);
#endif // FEATURE_OTA

    reset_arduino();

#ifdef FEATURE_STK500
    stk500_init(reset_arduino);
#endif

    init_web(reset_arduino);
#ifdef FEATURE_TCP
    init_tcp(dataBuffer, BUF_SIZE);
#endif
#ifdef FEATURE_WS
    init_ws();
#endif         
#ifdef FEATURE_UDP
    init_udp(dataBuffer, BUF_SIZE);
#endif
}

void loop() {
#ifdef FEATURE_OTA
    ArduinoOTA.handle();
#endif

    wifi_loop();
    mdns_loop();
#ifdef FEATURE_WS
    ws_loop();
#endif         
#ifdef FEATURE_UDP
    udp_loop();
#endif
#ifdef FEATURE_TCP
    tcp_loop();
#endif
    serial_loop();
    web_loop(wifi_is_captive());
}

void handle_serial(byte* buff, unsigned int n) {
#ifdef FEATURE_UDP
    udp_send(buff, n);
#endif
#ifdef FEATURE_TCP
    tcp_send(buff, n);
#endif
#ifdef FEATURE_WS
    ws_send(buff, n);
#endif         
}

void reset_arduino() {
    pinMode(RESET_PIN, OUTPUT_OPEN_DRAIN);
    digitalWrite(RESET_PIN, LOW);
    delay(10);
    digitalWrite(RESET_PIN, HIGH);
}

