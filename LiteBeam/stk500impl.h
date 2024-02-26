#ifndef __STK500IMPL_H__
#define __STK500IMPL_H__

#include <Esp.h>

#ifdef ESP32
#include <Wifi.h>
#else // ESP8266
#include <ESP8266WiFi.h>
#endif


void stk500_init(void (*reset)());
int handle_hex(WiFiServer *server, WiFiClient *client, int hexlen, int* written);


#endif //__STK500IMPL_H__
