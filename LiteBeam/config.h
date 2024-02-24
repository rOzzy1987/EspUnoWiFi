#ifndef __CONFIG_H__
#define __CONFIG_H__

/////////////////////// Features ///////////////////////
// Comment the lines to disable features and make the Firmware more compact
#define FEATURE_OTA                         // ESP OTA
#define FEATURE_STK500                      // Arduino OTA (experimental)
#define FEATURE_WS                          // WebSocket to serial bridge, including web serial monitor and GRBL pendant (come on, this is the heart of the project, don't disable it)
//#define FEATURE_UDP                         // UDP to serial bridge
//#define FEATURE_TCP                         // TCP to serial bridge

/////////////////////// User defined settings ///////////////////////
#define OTA_PASSWORD "GrblOTA"              // ESP OTA Update password (Comment to disable)
//#define OTA_PORT 8266                     // Custom ESP OTA port

//#define DEFAULT_ST_SSID "PrettyFly"       // Your network ssid. 
//#define DEAFULT_ST_PASSWORD "ForAWiFi"    // Your network password (if any)

//#define DEFAULT_AP_SSID "LiteBeam"        // Default AP network SSID
//#define DEFAULT_AP_PASSWORD ""            // Default AP network password (empty for open WiFi network)

//#define DEFAULT_HOSTNAME "LiteBeam"       // Default hostname for your device


/////////////////////// Service ports ///////////////////////
#define SRV_WEB_PORT 80                  
#define SRV_TCP_PORT 8080
#define SRV_WS_PORT 81
#define SRV_UDP_PORT 82


/////////////////////// Debugging ///////////////////////
//#define WIFI_DEBUG                        // Send serial messages to find out what WiFi network is hosted/being connected to 
//#define WS_DEBUG                          // Prints a "connected X" message on Serial when a WebSocket connection is created

/////////////////////// Pinout ///////////////////////
#define RESET_PIN 2                         // GPIO2 on ESP01; Connect to arduino reset pin


#endif //__CONFIG_H__