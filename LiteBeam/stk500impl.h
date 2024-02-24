#ifndef __STK500IMPL_H__
#define __STK500IMPL_H__

#include <ESP8266WiFi.h>


void stk500_init(void (*reset)());
int handle_hex(WiFiServer *server, WiFiClient *client, int hexlen, int* written);


#endif //__STK500IMPL_H__
