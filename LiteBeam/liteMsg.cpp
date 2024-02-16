#include "liteMsg.h"
#include <ESP8266WiFi.h>
#include "settings.h"
#include "common.h"

static WiFiServer msgServer(SRV_MESSAGE_PORT);
static WiFiClient msgClient;

static byte* msgBuff;
static unsigned int msgBuffSize;

void init_msg(byte* buff, unsigned int size) {
    msgServer.begin();
    msgBuff = buff;
    msgBuffSize = size;
}

void msg_loop() {
    unsigned int n;
    if (msgServer.hasClient()) {
        if (msgClient) 
            msgClient.stop();
        msgClient = msgServer.accept();
        msgClient.setNoDelay(true);
    }

    if (msgClient && msgClient.connected()) {
        while ((n = msgClient.available())) {
            n = msgClient.readBytes(msgBuff, Min(n, msgBuffSize));
            Serial.write(msgBuff, n);
            delay(0);
        }
    }
}

void msg_send(const byte *buff, const unsigned int n) {
    if (msgClient && msgClient.connected())
        msgClient.write(buff, n);
}