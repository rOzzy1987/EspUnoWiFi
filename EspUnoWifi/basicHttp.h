#ifndef __BASICHTTP_H__
#define __BASICHTTP_H__

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <WiFiUdp.h>
#include <math.h>
#include <pgmspace.h>


const char *head_frm =
    "HTTP/1.1 %d OK\r\n"
    "Content-Type: %s\r\n"
    "Access-Control-Allow-Origin:*\r\n"
    "Content-Length: %d\r\n"
    "Connection : close\r\n\r\n";

const char *ctHtml = "text/html";
const char *ctJs = "text/javascript"; 
const char *ctJson = "application/json"; 
const char *ctCss = "text/css"; 
const char *ctPlain = "text/plain"; 
const char *ctIco = "image/x-icon";



// somehow null is added at the end so we decrease size by 1
#define  fileResponse(wc, ctype, resource) wc.printf(head_frm, 200, ctype, sizeof(resource) - 1); \
                                           wc.write_P(resource, sizeof(resource) - 1);

#define  binFileResponse(wc, ctype, resource) wc.printf(head_frm, 200, ctype, sizeof(resource)); \
                                           wc.write_P((const char*)(void*)resource, sizeof(resource));


#endif