#ifndef __LITEWIFI_H__
#define __LITEWIFI_H__

/// @brief Sets up WiFi networking
void init_wifi();
bool wifi_is_captive();
void wifi_loop();

#endif // __LITEWIFI_H__
