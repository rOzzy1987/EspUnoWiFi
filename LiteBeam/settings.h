#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// EEPROM setting defaults
#ifndef DEFAULT_ST_SSID
#define DEFAULT_ST_SSID ""
#endif

#ifndef DEFAULT_ST_PASSWORD
#define DEFAULT_ST_PASSWORD ""
#endif

#ifndef DEFAULT_AP_PASSWORD
#define DEFAULT_AP_PASSWORD ""
#endif

#define FLAG_B0 0x23
#define FLAG_B1 0x3D

#include <EEPROM.h>
#include <Esp.h>

#define eeprom_size 512
typedef struct {
    char flag[2];
    char ssid[32];
    char pswd[32];
    char apssid[32];
    char appswd[32];
    unsigned int baudrate;
    unsigned int baudrate_isp;
    char hostname[32];
} SettingsStruct;

void init_eeprom();

void save_eeprom();

void load_eeprom(bool reset = false);
SettingsStruct* get_settings();

#endif //__SETTINGS_H__