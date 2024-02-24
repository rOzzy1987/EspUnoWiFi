#include <WebSocketsServer.h>
#include <Arduino.h>
#include "settings.h"
#include "liteWs.h"

static WebSocketsServer webSocket = WebSocketsServer(SRV_WS_PORT);

void init_ws() {
    webSocket.begin();
    webSocket.onEvent(ws_event);
}

void ws_loop() {
    webSocket.loop();
}

void ws_event(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
        case WStype_PING:
        case WStype_PONG:
            break;
        case WStype_CONNECTED:
#ifdef WS_DEBUG
            Serial.printf("Connected: %d\n", num);
#endif
            break;
        case WStype_TEXT:
        case WStype_BIN:
            Serial.write(payload, length);
            break;
    }
}

void ws_send(const byte* buff, unsigned int n) {
    webSocket.broadcastBIN(buff, n);
}