
#include "liteUdp.h"
#include <WiFiUdp.h>
#include "config.h"
#include "common.h"

static WiFiUDP udp;
static IPAddress udpAddress;
static IPAddress emptyAddress(0x00000000);
static unsigned int udpPort = 0;
static unsigned long udpLast = 0;


static byte* udpBuff;
static unsigned int udpBuffSize;

void init_udp(byte* buff, unsigned int size) {
    udp.begin(SRV_UDP_PORT);
    udpBuff = buff;
    udpBuffSize = size;
}

/// @brief Checks if there is any UDP packet in buffer. If there is, its content is forwarded 
/// to serial port, then the client's address is stored for 500ms
void udp_loop() {
    unsigned int n = 0;
    n = udp.parsePacket();
    if (n) {
        while (n) {
            int len = udp.read(udpBuff, Min(n, udpBuffSize));
            Serial.write(udpBuff, n);
            n -= len;
            delay(0);
        }
        udpPort = udp.remotePort();
        udpAddress = udp.remoteIP();
        udpLast = millis();
    } else if ((unsigned long)(millis() - udpLast) > 500) {
        udpPort = 0;
        udpAddress = emptyAddress;
    }
}

void udp_send(const byte* buff, const unsigned int n) {
    if (udpPort && udpAddress != emptyAddress) {
        udp.beginPacket(udpAddress, udpPort);
        udp.write(buff, n);
        udp.endPacket();
    }
    delay(0);
}
