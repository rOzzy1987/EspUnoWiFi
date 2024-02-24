#include <Arduino.h>
#include "settings.h"
#include "common.h"
#include "liteSerial.h"

static unsigned long last_time_tx = 0;
static unsigned int last_time_tx_available = 0;
static unsigned long last_time_tx_pass_tick = 0;

static byte *serialBuff;
static unsigned int serialBuffSize;
static void (*serial_callback) (byte* buff, unsigned int n);

void init_serial(byte* buff, unsigned int size, void (*callback_fn) (byte* buff, unsigned int n)) {
    SettingsStruct *s = get_settings();
    Serial.begin(s->baudrate);
    serialBuff = buff;
    serialBuffSize = size;
    serial_callback = callback_fn;
}

void serial_loop() {
    unsigned int n = Serial.available();
    if (n != last_time_tx_available) {
        last_time_tx_pass_tick = millis();
        last_time_tx_available = n;
    }

    bool tx =  (millis() - last_time_tx > 30)
            || (millis() - last_time_tx_pass_tick > 2);
    tx = tx && n;
    tx = tx || (n > sizeof(serialBuff) / 2);
    if (tx) {
        do {
            n = Serial.available();
            n = Serial.readBytes(
                serialBuff, Min(n, serialBuffSize));

            serial_callback(serialBuff, n);
        } while (Serial.available() > (int)serialBuffSize / 4);
        last_time_tx = millis();
    }
}