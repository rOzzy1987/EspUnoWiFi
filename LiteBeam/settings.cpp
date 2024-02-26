#include "settings.h"
#include <EEPROM.h>
#include <Esp.h>


#ifdef ESP32
#include <Wifi.h>
#define chipId() WiFi.macAddress().c_str()
#else // ESP8266
#define chipId() ESP.getChipId()
#endif

static SettingsStruct liteSettings;

void init_eeprom() {
    EEPROM.begin(eeprom_size);

    load_eeprom();
}

void save_eeprom() {
    EEPROM.put(0, liteSettings);
    EEPROM.commit();
}

void load_eeprom(bool reset) {
    EEPROM.get(0, liteSettings);

    if (liteSettings.flag[0] != FLAG_B0 || liteSettings.flag[1] != FLAG_B1 || reset) {
        memset(&liteSettings, 0, sizeof(liteSettings));
        liteSettings.flag[0] = FLAG_B0;
        liteSettings.flag[1] = FLAG_B1;

#ifdef DEFAULT_AP_SSID
        if (strlen(AP_SSID) >= 2)
            strcpy(liteSettings.apssid, DEFAULT_AP_SSID);
        else
            sprintf_P(liteSettings.apssid, PSTR("LiteBeam-%06x"), chipId());
#else
        sprintf_P(liteSettings.apssid, PSTR("LiteBeam-%06x"), chipId());
#endif

        strcpy(liteSettings.appswd, DEFAULT_AP_PASSWORD);


    
#ifdef DEFAULT_HOSTNAME
        strcpy(liteSettings.hostname, DEFAULT_HOSTNAME);
#else
        sprintf_P(liteSettings.hostname, PSTR("LiteBeam-%06x"), chipId());
#endif

        strcpy(liteSettings.ssid, DEFAULT_ST_SSID);
        strcpy(liteSettings.pswd, DEFAULT_ST_PASSWORD);


        save_eeprom();
    }
    if (liteSettings.baudrate == 0 || liteSettings.baudrate > 2000000)
        liteSettings.baudrate = 115200;
    if (liteSettings.baudrate_isp == 0 || liteSettings.baudrate_isp > 2000000)
        liteSettings.baudrate_isp = 115200;
    
}


SettingsStruct* get_settings() {
    return &liteSettings;
}