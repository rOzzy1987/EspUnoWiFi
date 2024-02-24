#ifndef __LITEWS_H__
#define __LITEWS_H__

#include <WebSocketsServer.h>

void init_ws();
void ws_loop();
void ws_event(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void ws_send(const byte* buff, unsigned int n);

#endif // __LITEWS_H__
