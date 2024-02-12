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
#include "basicHttp.h"

const char default_router_ssid[] = "";  // your router ssid
const char default_router_pswd[] = "";  // your router password

#define eeprom_size 512
#pragma pack(push)
#pragma pack(1)
typedef struct {
    char flag[2];
    char ssid[32];
    char pswd[32];
    char apssid[32];
    char appswd[32];
    unsigned int baudrate;
    unsigned int baudrate_isp;
} EEPROM_CONFIG;
EEPROM_CONFIG econfig;
#pragma pack(pop)

const int resetPin = 2;  // GPIO2-->UNO reset pin
void resetTarget() {
    pinMode(resetPin, OUTPUT_OPEN_DRAIN);
    digitalWrite(resetPin, LOW);
    delay(10);
    digitalWrite(resetPin, HIGH);
}

unsigned char read_page_buff[128];
unsigned char write_page_buff[sizeof(read_page_buff)];
int address = 0;
int baseaddr = 0;
int total_write = 0;
uint8_t dity = 0;

WiFiServer avrOTA(80);
WiFiClient avrOTA_Clients[8];
unsigned int avrOTA_Client_LastIndex = 0;

WiFiServer msgserver(8080);
WiFiClient msgclient;

WiFiUDP Udp;
IPAddress udp_address;
unsigned int udp_port = 0;
unsigned long udp_last = 0;

WebSocketsServer webSocket = WebSocketsServer(81);

//#define debug(fmt, ...)
// if(msgclient)if(msgclient.connected())msgclient.printf(fmt, ##__VA_ARGS__)
#define debug(...)

inline uint8_t ctoh(char c) {
    return (c & 0x10) ? /*0-9*/ c & 0xf : /*A-F, a-f*/ (c & 0xf) + 9;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            break;
        case WStype_CONNECTED:
            webSocket.sendTXT(num, "Connected\n");
            break;
        case WStype_TEXT:
        case WStype_BIN:
            Serial.write(payload, length);
            break;
    }
}

void save_eeprom() {
    EEPROM.put(0, econfig);
    EEPROM.commit();
}

void load_eeprom() {
    EEPROM.get(0, econfig);

    if (econfig.flag[0] != 0xaa || econfig.flag[1] != 0x55) {
        memset(&econfig, 0, sizeof(econfig));
        econfig.flag[0] = 0xaa;
        econfig.flag[1] = 0x55;
        save_eeprom();
    }
    if (econfig.baudrate == 0 || econfig.baudrate > 2000000)
        econfig.baudrate = 115200;
    if (econfig.baudrate_isp == 0 || econfig.baudrate_isp > 2000000)
        econfig.baudrate_isp = 115200;
    if (strlen(econfig.apssid) == 0) {
        sprintf_P(econfig.apssid, PSTR("WIFI-ESP-SERIAL-%06x"),
                  ESP.getChipId());
        // strcpy_P(econfig.appswd, PSTR("oseppesp"));
    }

    if (strlen(econfig.ssid) == 0) {
        strcpy(econfig.ssid, default_router_ssid);
        strcpy(econfig.pswd, default_router_pswd);
    }
}

void setup() {
    EEPROM.begin(eeprom_size);
    load_eeprom();
    Serial.begin(econfig.baudrate);
    Serial.print("ssid:");
    Serial.println(econfig.ssid);
    Serial.print("pswd:");
    Serial.println(econfig.pswd);
    WiFi.mode(WIFI_AP_STA);
    if (strlen(econfig.apssid)) {
        if (strlen(econfig.appswd) < 8)
            WiFi.softAP(econfig.apssid);
        else
            WiFi.softAP(econfig.apssid, econfig.appswd);
        WiFi.setHostname(econfig.apssid);
    }
    if (strlen(econfig.ssid)) {
        if (strlen(econfig.pswd))
            WiFi.begin(econfig.ssid, econfig.pswd);
        else
            WiFi.begin(econfig.ssid);

        if (WiFi.status() != WL_CONNECTED) delay(500);
    }
    resetTarget();
    ArduinoOTA.begin(false);

    avrOTA.begin();
    msgserver.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    Udp.begin(82);
}

void netstat_json(WiFiClient* wc) {
    wc->setTimeout(10000);
    int dlen = 
        (strlen(econfig.apssid) -2) +
        (strlen(econfig.ssid) -2) +
        (strlen(econfig.appswd) -2);

    const char* json = R"=====({"ap": {"ssid": "%s", "password": "%s"}, "client": {"ssid": "%s", "password":""}, "status": {"available": [%s], "ip": "%s", "subnet": "%s", "gateway": "%s", "dns": "%s"}})=====";

    int n = WiFi.scanNetworks();
        
    String available = "";
    for (int i = 0; i < n; ++i) {
      String ssid = WiFi.SSID(i);
      ssid.replace('"','.');

        available += "\"" + ssid + "\"";
        if (i < n-1)
            available += ",";
    }
    dlen += available.length() - 2;

    String localip = WiFi.localIP().toString();
    dlen += localip.length() - 2;

    String subnet = WiFi.subnetMask().toString();
    dlen += subnet.length() - 2;

    String gateway = WiFi.gatewayIP().toString();
    dlen += gateway.length() - 2;

    String dns = WiFi.dnsIP().toString();
    dlen += dns.length() - 2;

    wc->printf(head_frm, 200, ctJson, strlen(json) + dlen);
    wc->printf(json, econfig.apssid, econfig.appswd, econfig.ssid, available.c_str(),
                localip.c_str(), subnet.c_str(), gateway.c_str(), dns.c_str());
}

void setssid(byte isAP, WiFiClient* wc) {
    String line;
    int contentLen = 0;
    do {
        line = wc->readStringUntil('\n');
        line.toLowerCase();
        if (line.indexOf("content-length") >= 0) {
            contentLen = line.substring(line.indexOf(':') + 1).toInt();
        }
    } while (line != String("\r"));

    char key[256];
    int pos = 0;
    char *value = NULL;
    char tssid[32];
    char tpswd[32];
    memset(tssid, 0, sizeof(tssid));
    memset(tpswd, 0, sizeof(tpswd));
    while (contentLen) {
        char c = wc->read();
        contentLen--;
        if (c == '&' || contentLen == 0) {
            if (c != '&') key[pos++] = c;
            key[pos] = '\0';
            if (strstr_P(key, PSTR("ssid"))) {
                if (value) {
                    for (int i = 0; i < sizeof(tssid) - 1; i++) {
                        tssid[i] = value[i];
                        if (value[i] == '\0') break;
                    }
                }
            } else if (strstr_P(key, PSTR("pswd"))) {
                if (value) {
                    for (int i = 0; i < sizeof(tpswd) - 1; i++) {
                        tpswd[i] = value[i];
                        if (value[i] == '\0') break;
                    }
                }
            }
            pos = 0;
            key[0] = '\0';
            value = NULL;
        } else if (c == '%') {
            c = ctoh(wc->read());
            c <<= 4;
            c |= ctoh(wc->read()) & 0x0f;
            contentLen -= 2;
            key[pos++] = c;
        } else if (c == '=') {
            key[pos++] = '\0';
            value = key + pos;
        } else if ((c != '\r') && (c != '\n')) {
            key[pos++] = c;
        }
        if (pos == sizeof(key)) break;
    }

    fileResponse((*wc), ctHtml, res_redir_html);

    save_eeprom();
    if (isAP) {
        strcpy(econfig.apssid, tssid);
        strcpy(econfig.appswd, tpswd);
        WiFi.softAPdisconnect();
        if (strlen(tssid)) {
            if (strlen(tpswd) < 7)
                WiFi.softAP(tssid);
            else
                WiFi.softAP(tssid, tpswd);
        }
    } else {
        strcpy(econfig.ssid, tssid);
        strcpy(econfig.pswd, tpswd);
        WiFi.disconnect();
        if (strlen(tssid)) {
            if (strlen(tpswd))
                WiFi.begin(tssid);
            else
                WiFi.begin(tssid, tpswd);
        }
    }
    MDNS.notifyAPChange();
    MDNS.update();
    save_eeprom();
    while (wc->availableForWrite()) wc->flush();
    ESP.reset();
}

byte msgPipeBuff[512];
unsigned long last_time_tx = 0;
int last_time_tx_available = 0;
unsigned long last_time_tx_pass_tick = 0;
void loop() {
    static bool msdn_inited = false;
    if (!msdn_inited && WiFi.status() == WL_CONNECTED) {
        if (MDNS.begin(econfig.apssid)) {
            MDNS.enableArduino(8266);
            MDNS.addService("websock", "tcp", 81);
            MDNS.addService("message", "tcp", 8080);
            MDNS.addService("http", "tcp", 80);
            MDNS.addService("remotecontrol", "udp", 82);
            MDNS.update();
            msdn_inited = true;
        }
    }
    if (msdn_inited) MDNS.update();
    ArduinoOTA.handle();

    webSocket.loop();
    int n = 0;
    n = Udp.parsePacket();
    if (n) {
        while (n) {
            int len = Udp.read(
                msgPipeBuff, n > sizeof(msgPipeBuff) ? sizeof(msgPipeBuff) : n);
            Serial.write(msgPipeBuff, n);
            n -= len;
            delay(0);
        }
        udp_port = Udp.remotePort();
        udp_address = Udp.remoteIP();
        udp_last = millis();
    } else {
        if ((unsigned long)(millis() - udp_last) > 500) udp_port = 0;
    }

    if (msgserver.hasClient()) {
        if (msgclient) msgclient.stop();
        msgclient = msgserver.available();
        msgclient.setNoDelay(true);
    }

    if (msgclient && msgclient.connected()) {
        while (n = msgclient.available()) {
            n = msgclient.readBytes(
                msgPipeBuff, n > sizeof(msgPipeBuff) ? sizeof(msgPipeBuff) : n);
            Serial.write(msgPipeBuff, n);
            delay(0);
        }
    }
    n = Serial.available();
    if (n != last_time_tx_available) {
        last_time_tx_pass_tick = millis();
        last_time_tx_available = n;
    }
    bool tx = (millis() - last_time_tx > 30);
    tx = tx || (millis() - last_time_tx_pass_tick > 2);
    tx = tx && n;
    tx = tx || (n > sizeof(msgPipeBuff) / 2);
    if (tx) {
        do {
            n = Serial.available();
            n = Serial.readBytes(
                msgPipeBuff, n > sizeof(msgPipeBuff) ? sizeof(msgPipeBuff) : n);
            if (msgclient && msgclient.connected())
                msgclient.write(msgPipeBuff, n);
            webSocket.broadcastBIN(msgPipeBuff, n);
            if (udp_port) {
                Udp.beginPacket(udp_address, udp_port);
                Udp.write(msgPipeBuff, n);
                Udp.endPacket();
            }
            delay(0);
        } while (Serial.available() > sizeof(msgPipeBuff) / 4);
        last_time_tx = millis();
    }

    if (avrOTA.hasClient()) {
        avrOTA_Client_LastIndex++;
        int index;
        for (int i = 0; i < sizeof(avrOTA_Clients) / sizeof(avrOTA_Clients[0]);
             i++, avrOTA_Client_LastIndex++) {
            index = avrOTA_Client_LastIndex %
                    (sizeof(avrOTA_Clients) / sizeof(avrOTA_Clients[0]));
            if (!avrOTA_Clients[index]) break;
            if (!avrOTA_Clients[index].connected()) break;
        }

        avrOTA_Client_LastIndex %=
            (sizeof(avrOTA_Clients) / sizeof(avrOTA_Clients[0]));
        if (avrOTA_Clients[avrOTA_Client_LastIndex])
            avrOTA_Clients[avrOTA_Client_LastIndex].stop();
        avrOTA_Clients[avrOTA_Client_LastIndex] = avrOTA.available();
        avrOTA_Clients[avrOTA_Client_LastIndex].setNoDelay(true);
    }
    for (int i = 1; i <= sizeof(avrOTA_Clients) / sizeof(avrOTA_Clients[0]);
         i++) {
        WiFiClient avrOTA_client =
            avrOTA_Clients[(avrOTA_Client_LastIndex + i) %
                           (sizeof(avrOTA_Clients) /
                            sizeof(avrOTA_Clients[0]))];
        if (avrOTA_client && avrOTA_client.connected() &&
            avrOTA_client.available()) {
            String s = avrOTA_client.readStringUntil('\n');
            s.toLowerCase();

            // Control related
            int pos = s.lastIndexOf("baudrate=");
            if (pos > 0) {
                while (s[pos++] != '=')
                    ;
                unsigned int trate = 0;
                while (s[pos] >= '0' && s[pos] <= '9') {
                    trate *= 10;
                    trate += s[pos] - '0';
                    pos++;
                }
                if (trate > 0 && trate <= 2000000) {
                    if (econfig.baudrate != trate) {
                        econfig.baudrate = trate;
                        save_eeprom();
                        Serial.begin(trate);
                    }
                }
            }
            pos = s.lastIndexOf("baudrateisp=");
            if (pos > 0) {
                while (s[pos++] != '=')
                    ;
                unsigned int trate = 0;
                while (s[pos] >= '0' && s[pos] <= '9') {
                    trate *= 10;
                    trate += s[pos] - '0';
                    pos++;
                }
                if (trate > 0 && trate <= 2000000) {
                    if (econfig.baudrate_isp != trate) {
                        econfig.baudrate_isp = trate;
                        save_eeprom();
                    }
                }
            }
            if (s.indexOf("sync") > 0) {
                String respone("Esp8266 for UnoOTA, SYNC OK!");
                respone += millis();
                if (s.indexOf("post") >= 0) {
                    avrOTA_client.printf(head_frm, 204, ctPlain, respone.length());
                    avrOTA_client.print(respone);
                } else {
                    avrOTA_client.printf(head_frm, 200, ctPlain, respone.length());
                    avrOTA_client.print(respone);
                }
            } else if (s.indexOf("reset") > 0) {
                resetTarget();
                String respone("Reset OK!");
                respone += millis();
                avrOTA_client.printf(head_frm, 200, ctPlain, respone.length());
                avrOTA_client.print(respone);
            } else if (s.indexOf("upload") > 0) {
                String line;
                int hexLen = 0;
                do {
                    if (!avrOTA_client.available()) break;
                    line = avrOTA_client.readStringUntil('\n');
                    line.toLowerCase();
                    if (line.indexOf("content-length") >= 0) {
                        hexLen = line.substring(line.indexOf(':') + 1).toInt();
                    }
                } while (line != String("\r"));
                extern int handleHex(WiFiClient client, int hexlen);
                int handLen = handleHex(avrOTA_client, hexLen);
                Serial.begin(econfig.baudrate);
                if (handLen != hexLen) {
                    String respone("Upload Error!");
                    if (handLen < 0) {
                        respone += " can't entry bootloader!";
                    } else {
                        respone += " at Hex File(byte):";
                        respone += handLen;
                        respone += " total:";
                        respone += hexLen;
                        respone += ",";
                        respone += total_write;
                        respone += " bytes have been written";
                    }
                    avrOTA_client.printf(head_frm, 404, ctPlain, respone.length());
                    avrOTA_client.print(respone);
                } else {
                    String respone("Upload done! Flash used:");
                    respone += total_write;
                    respone += " bytes";
                    avrOTA_client.printf(head_frm, 200, ctPlain, respone.length());
                    avrOTA_client.print(respone);
                }

            // HTML content related

            } else if (s.indexOf("favicon.ico") > 0) {
                binFileResponse(avrOTA_client, ctIco, res_favicon_ico);

            } else if (s.indexOf("common.js") > 0) {
                fileResponse(avrOTA_client, ctJs, res_common_js);
            } else if (s.indexOf("netstat.js") > 0) {
                fileResponse(avrOTA_client, ctJs, res_netstat_js);
            } else if (s.indexOf("pendant.js") > 0) {
                fileResponse(avrOTA_client, ctJs, res_pendant_js);
            } else if (s.indexOf("serial.js") > 0) {
                fileResponse(avrOTA_client, ctJs, res_serial_js);
            } else if (s.indexOf("ui.js") > 0) {
                fileResponse(avrOTA_client, ctJs, res_ui_js);

            } else if (s.indexOf("ui.css") > 0) {
                fileResponse(avrOTA_client, ctCss, res_ui_css);

            } else if (s.indexOf("netstat.html") > 0) {
                fileResponse(avrOTA_client, ctHtml, res_netstat_html);
            } else if (s.indexOf("pendant.html") > 0) {
                fileResponse(avrOTA_client, ctHtml, res_pendant_html);
            } else if (s.indexOf("redir.html") > 0) {
                fileResponse(avrOTA_client, ctHtml, res_redir_html);
            } else if ((s.indexOf("index") > 0) || (s.indexOf(" / ") > 0) ||
                       (s.indexOf("serial.html") > 0)) {
                fileResponse(avrOTA_client, ctHtml, res_serial_html);

            // Actions
            } else if ((s.indexOf("setap") > 0) && (s.indexOf("post") >= 0)) {
                setssid(1, &avrOTA_client);
            } else if ((s.indexOf("setwf") > 0) && (s.indexOf("post") >= 0)) {
                setssid(0, &avrOTA_client);
            } else if (s.indexOf("netstat") > 0) {
                netstat_json(&avrOTA_client);
            } else if (s.indexOf("getbaudrate") > 0) {
                String respone(econfig.baudrate);
                avrOTA_client.printf(head_frm, 200, ctJson, respone.length());
                avrOTA_client.print(respone);

            // 404
            } else {
                avrOTA_client.printf(head_frm, 404, ctPlain, 0);
            }
            avrOTA_client.flush();
            // avrOTA_client.stop();
        }
    }
}

#include "stk500.h"

#define MAX_SYNC_ATTEMPTS 10

static int stk500_send(unsigned char *buf, size_t len) {
    Serial.write(buf, len);
    return len;
}
static int stk500_send(unsigned char data) {
    Serial.write(data);
    return 1;
}

static int stk500_recv(unsigned char *buf, size_t len) {
    int i = 200;
    while (i && Serial.available() < (int)len) {
        delay(1);
        i--;
    }
    int rv = Serial.readBytes(buf, len);
    if (rv < 0) {
        debug("stk500_recv(): programmer is not responding\n");
        return -1;
    }

    return 0;
}

int stk500_drain() {
    while (Serial.available()) Serial.read();
    return 0;
}

int stk500_getsync() {
    unsigned char buf[32], resp[32];
    int attempt;

    /* get in sync */
    buf[0] = Cmnd_STK_GET_SYNC;
    buf[1] = Sync_CRC_EOP;

    /*
        First send and drain a few times to get rid of line noise
    */

    stk500_send(buf, 2);
    stk500_drain();
    stk500_send(buf, 2);
    stk500_drain();

    for (attempt = 0; attempt < MAX_SYNC_ATTEMPTS; attempt++) {
        stk500_send(buf, 2);
        stk500_recv(resp, 1);
        if (resp[0] == Resp_STK_INSYNC) {
            break;
        }
        debug("stk500_getsync() attempt %d of %d: not in sync: resp=0x%02x\n",
              attempt + 1, MAX_SYNC_ATTEMPTS, resp[0]);
    }
    if (attempt == MAX_SYNC_ATTEMPTS) {
        stk500_drain();
        return -1;
    }

    if (stk500_recv(resp, 1) < 0) return -1;
    if (resp[0] != Resp_STK_OK) {
        debug("stk500_getsync(): can't communicate with device: resp=0x%02x\n",
              resp[0]);
        return -1;
    }

    return 0;
}

static int stk500_loadaddr(unsigned int addr) {
    unsigned char buf[16];
    int tries;
    tries = 0;
retry:
    tries++;
    buf[0] = Cmnd_STK_LOAD_ADDRESS;
    buf[1] = addr & 0xff;
    buf[2] = (addr >> 8) & 0xff;
    buf[3] = Sync_CRC_EOP;

    stk500_send(buf, 4);

    if (stk500_recv(buf, 1) < 0) return -1;
    if (buf[0] == Resp_STK_NOSYNC) {
        if (tries > 33) {
            debug("stk500_loadaddr(): can't get into sync\n");
            return -1;
        }
        if (stk500_getsync() < 0) return -1;
        goto retry;
    } else if (buf[0] != Resp_STK_INSYNC) {
        debug(
            "stk500_loadaddr(): (a) protocol error, expect=0x%02x, "
            "resp=0x%02x\n",
            Resp_STK_INSYNC, buf[0]);
        return -1;
    }

    if (stk500_recv(buf, 1) < 0) return -1;
    if (buf[0] == Resp_STK_OK) {
        return 0;
    }

    debug("loadaddr(): (b) protocol error, expect=0x%02x, resp=0x%02x\n",
          Resp_STK_INSYNC, buf[0]);

    return -1;
}

static int stk500_paged_write(unsigned int addr, unsigned int n_bytes,
                              unsigned char *data) {
    unsigned char buf[16];

    int a_div;
    int block_size;
    int tries;
    unsigned int n;

    a_div = 2;
    block_size = 128;

    n = addr + n_bytes;

    for (; addr < n; addr += block_size) {
        tries = 0;
    retry:
        tries++;
        stk500_loadaddr(addr / a_div);
        stk500_send(Cmnd_STK_PROG_PAGE);
        stk500_send((block_size >> 8) & 0xff);
        stk500_send(block_size & 0xff);
        stk500_send('F');
        stk500_send(data, block_size);
        stk500_send(Sync_CRC_EOP);
        data += block_size;
        if (stk500_recv(buf, 1) < 0) return -1;
        if (buf[0] == Resp_STK_NOSYNC) {
            if (tries > 33) {
                debug("stk500_paged_write(): can't get into sync\n");
                return -3;
            }
            if (stk500_getsync() < 0) return -1;
            goto retry;
        } else if (buf[0] != Resp_STK_INSYNC) {
            debug(
                "stk500_paged_write(): (a) protocol error, expect=0x%02x, "
                "resp=0x%02x\n",
                Resp_STK_INSYNC, buf[0]);
            return -4;
        }

        if (stk500_recv(buf, 1) < 0) return -1;
        if (buf[0] != Resp_STK_OK) {
            debug(
                "stk500_paged_write(): (a) protocol error, expect=0x%02x, "
                "resp=0x%02x\n",
                Resp_STK_INSYNC, buf[0]);
            return -5;
        }
    }
    return n_bytes;
}

static int stk500_paged_load(unsigned int addr, unsigned int n_bytes,
                             unsigned char *data) {
    unsigned char buf[16];
    int a_div;
    int tries;
    unsigned int n;
    int block_size;

    a_div = 2;
    block_size = 128;

    n = addr + n_bytes;
    for (; addr < n; addr += block_size) {
        tries = 0;
    retry:
        tries++;
        stk500_loadaddr(addr / a_div);
        stk500_send(Cmnd_STK_READ_PAGE);
        stk500_send((block_size >> 8) & 0xff);
        stk500_send(block_size & 0xff);
        stk500_send('F');
        stk500_send(Sync_CRC_EOP);

        if (stk500_recv(buf, 1) < 0) return -1;
        if (buf[0] == Resp_STK_NOSYNC) {
            if (tries > 33) {
                debug("stk500_paged_load(): can't get into sync\n");
                return -3;
            }
            if (stk500_getsync() < 0) return -1;
            goto retry;
        } else if (buf[0] != Resp_STK_INSYNC) {
            debug(
                "stk500_paged_load(): (a) protocol error, "
                "expect=0x%02x, resp=0x%02x\n",
                Resp_STK_INSYNC, buf[0]);
            return -4;
        }

        if (stk500_recv(data, block_size) < 0) return -1;
        data += block_size;
        if (stk500_recv(buf, 1) < 0) return -1;

        if (buf[0] != Resp_STK_OK) {
            debug(
                "stk500_paged_load(): (a) protocol error, expect=0x%02x, "
                "resp=0x%02x\n",
                Resp_STK_OK, buf[0]);
            return -5;
        }
    }
    return n_bytes;
}

int pageCheck(int curAddress) {
    int page = address / sizeof(write_page_buff);
    int curpage = curAddress / sizeof(write_page_buff);
    if (page != curpage) {
        if (dity) {
            debug("write page:%04X\n", page * sizeof(write_page_buff));
            int wb =
                stk500_paged_write(page * sizeof(write_page_buff),
                                   sizeof(write_page_buff), write_page_buff);
            if (wb != sizeof(write_page_buff)) return -1;
            int rb = stk500_paged_load(page * sizeof(write_page_buff),
                                       sizeof(read_page_buff), read_page_buff);
            if (rb != sizeof(write_page_buff)) return -1;
            int err = 0;
            for (unsigned int ck = 0; ck < sizeof(write_page_buff); ck++)
                if (read_page_buff[ck] != write_page_buff[ck]) err++;
            if (err) return -1;
        }
        address = curAddress;
        memset(read_page_buff, 0xff, sizeof(read_page_buff));
        memset(write_page_buff, 0xff, sizeof(write_page_buff));
        dity = 0;
    }
    return 0;
}

int entryBootloader() {
    Serial.begin(econfig.baudrate_isp);
    resetTarget();
    delay(500);
    int i = 0;
    while (1) {
        while (Serial.available()) Serial.read();
        Serial.write(0x30);
        Serial.write(0x20);
        delay(10);
        i++;
        if (i > 500 / 10) {
            return 0;
        }
        if (Serial.available() > 1) {
            if (Serial.read() != 0x14) continue;
            if (Serial.read() != 0x10) continue;
            return 1;
        }
    }
}

int sigleRecord(uint8_t *data, int len) {
    uint8_t cs = 0;
    uint8_t recsize = ctoh(*data++);
    recsize <<= 4;
    recsize |= ctoh(*data++);
    cs = recsize;

    if ((recsize * 2 + (1 + 2 + 1 + 1) * 2) != len) return -1;

    uint16_t addr = ctoh(*data++);
    addr <<= 4;
    addr |= ctoh(*data++);
    cs += (uint8_t)addr;

    addr <<= 4;
    addr |= ctoh(*data++);
    addr <<= 4;
    addr |= ctoh(*data++);
    cs += (uint8_t)(addr & 0xff);

    uint8_t rectype = ctoh(*data++);
    rectype <<= 4;
    rectype |= ctoh(*data++);
    cs += rectype;
    debug("addr:%04X :", addr);
    uint8_t *phexdata = data;
    for (int i = 0; i < recsize; i++) {
        uint8_t d = ctoh(*data++);
        d <<= 4;
        d |= ctoh(*data++);
        cs += d;
        phexdata[i] = d;
    }
    uint8_t csd = ctoh(*data++);
    csd <<= 4;
    csd |= ctoh(*data++);
    cs += csd;
    if (cs) return -3;

    switch (rectype) {
        case 0:
            for (int i = 0; i < recsize; i++) {
                if (pageCheck(baseaddr + addr + i)) return -2;
                write_page_buff[(baseaddr + addr + i) %
                                sizeof(write_page_buff)] = phexdata[i];
            }
            debug("\n");
            if (recsize) dity = 1;
            total_write += recsize;
            break;
        case 1:
            if (pageCheck(address + sizeof(write_page_buff))) return -1;
            break;
        case 2:
            baseaddr = (phexdata[0] << 8 | phexdata[1]) << 4;
            return 0;
        case 3:
            return 0;
        case 4:
            baseaddr = (phexdata[0] << 8 | phexdata[1]) << 16;
            return 0;
        case 5:
            return 0;
        default:
            return rectype;
            break;
    }
    return rectype;
}

uint8_t lineBuff[128];
int handleHex(WiFiClient client, int hexlen) {
    while (client.connected() && client.available() == 0) {
        delay(1);
    }
    if (client.read() != ':') return 0;
    int totalLen = 0;
    address = -1;
    baseaddr = 0;
    dity = 0;
    total_write = 0;
    memset(read_page_buff, 0xff, sizeof(read_page_buff));
    memset(write_page_buff, 0xff, sizeof(write_page_buff));
    if (!entryBootloader())
        if (!entryBootloader())
            if (!entryBootloader()) return -1;
    while (client.connected()) {
        ArduinoOTA.handle();
        if (avrOTA.hasClient()) break;
        int rl = client.readBytesUntil(
            ':', lineBuff,
            min((int)sizeof(lineBuff) - 1, hexlen - totalLen - 1));
        int dlen = rl;
        if (dlen < 10) break;
        if (lineBuff[rl - 1] == '\n') dlen--;
        if (lineBuff[rl - 2] == '\r') dlen--;
        if (dlen < 10) break;
        if (dlen & 1) break;
        totalLen += rl + 1;
        if (sigleRecord(lineBuff, dlen)) break;
    }
    return totalLen;
}
