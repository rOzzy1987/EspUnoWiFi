#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include "stk500.h"
#include "settings.h"
#include "common.h"

//#define debug(fmt, ...)
// if(msgclient)if(msgclient.connected())msgclient.printf(fmt, ##__VA_ARGS__)
#define debug(...)

#define MAX_SYNC_ATTEMPTS 10

static void (*stk500impl_reset)();

static unsigned char read_page_buff[128];
static unsigned char write_page_buff[sizeof(read_page_buff)];
static int address = 0;
static int baseaddr = 0;
static int total_write = 0;
static uint8_t dity = 0;
static uint8_t lineBuff[128];

void stk500_init(void (*reset)()){
    stk500impl_reset = reset;
}

static int stk500_send(unsigned char *buf, size_t len) {
    Serial.write(buf, len);
    return len;
}
static int stk500_send(unsigned char data) {
    Serial.write(data);
    return 1;
}

static int stk500_recv(unsigned char *buf, size_t len) {
    int i = 200;
    while (i && Serial.available() < (int)len) {
        delay(1);
        i--;
    }
    int rv = Serial.readBytes(buf, len);
    if (rv < 0) {
        debug("stk500_recv(): programmer is not responding\n");
        return -1;
    }

    return 0;
}

static int stk500_drain() {
    while (Serial.available()) Serial.read();
    return 0;
}

static int stk500_getsync() {
    unsigned char buf[32], resp[32];
    int attempt;

    /* get in sync */
    buf[0] = Cmnd_STK_GET_SYNC;
    buf[1] = Sync_CRC_EOP;

    /*
        First send and drain a few times to get rid of line noise
    */

    stk500_send(buf, 2);
    stk500_drain();
    stk500_send(buf, 2);
    stk500_drain();

    for (attempt = 0; attempt < MAX_SYNC_ATTEMPTS; attempt++) {
        stk500_send(buf, 2);
        stk500_recv(resp, 1);
        if (resp[0] == Resp_STK_INSYNC) {
            break;
        }
        debug("stk500_getsync() attempt %d of %d: not in sync: resp=0x%02x\n",
              attempt + 1, MAX_SYNC_ATTEMPTS, resp[0]);
    }
    if (attempt == MAX_SYNC_ATTEMPTS) {
        stk500_drain();
        return -1;
    }

    if (stk500_recv(resp, 1) < 0) return -1;
    if (resp[0] != Resp_STK_OK) {
        debug("stk500_getsync(): can't communicate with device: resp=0x%02x\n",
              resp[0]);
        return -1;
    }

    return 0;
}

static int stk500_loadaddr(unsigned int addr) {
    unsigned char buf[16];
    int tries;
    tries = 0;
retry:
    tries++;
    buf[0] = Cmnd_STK_LOAD_ADDRESS;
    buf[1] = addr & 0xff;
    buf[2] = (addr >> 8) & 0xff;
    buf[3] = Sync_CRC_EOP;

    stk500_send(buf, 4);

    if (stk500_recv(buf, 1) < 0) return -1;
    if (buf[0] == Resp_STK_NOSYNC) {
        if (tries > 33) {
            debug("stk500_loadaddr(): can't get into sync\n");
            return -1;
        }
        if (stk500_getsync() < 0) return -1;
        goto retry;
    } else if (buf[0] != Resp_STK_INSYNC) {
        debug(
            "stk500_loadaddr(): (a) protocol error, expect=0x%02x, "
            "resp=0x%02x\n",
            Resp_STK_INSYNC, buf[0]);
        return -1;
    }

    if (stk500_recv(buf, 1) < 0) return -1;
    if (buf[0] == Resp_STK_OK) {
        return 0;
    }

    debug("loadaddr(): (b) protocol error, expect=0x%02x, resp=0x%02x\n",
          Resp_STK_INSYNC, buf[0]);

    return -1;
}

static int stk500_paged_write(unsigned int addr, unsigned int n_bytes,
                              unsigned char *data) {
    unsigned char buf[16];

    int a_div;
    int block_size;
    int tries;
    unsigned int n;

    a_div = 2;
    block_size = 128;

    n = addr + n_bytes;

    for (; addr < n; addr += block_size) {
        tries = 0;
    retry:
        tries++;
        stk500_loadaddr(addr / a_div);
        stk500_send(Cmnd_STK_PROG_PAGE);
        stk500_send((block_size >> 8) & 0xff);
        stk500_send(block_size & 0xff);
        stk500_send('F');
        stk500_send(data, block_size);
        stk500_send(Sync_CRC_EOP);
        data += block_size;
        if (stk500_recv(buf, 1) < 0) return -1;
        if (buf[0] == Resp_STK_NOSYNC) {
            if (tries > 33) {
                debug("stk500_paged_write(): can't get into sync\n");
                return -3;
            }
            if (stk500_getsync() < 0) return -1;
            goto retry;
        } else if (buf[0] != Resp_STK_INSYNC) {
            debug(
                "stk500_paged_write(): (a) protocol error, expect=0x%02x, "
                "resp=0x%02x\n",
                Resp_STK_INSYNC, buf[0]);
            return -4;
        }

        if (stk500_recv(buf, 1) < 0) return -1;
        if (buf[0] != Resp_STK_OK) {
            debug(
                "stk500_paged_write(): (a) protocol error, expect=0x%02x, "
                "resp=0x%02x\n",
                Resp_STK_INSYNC, buf[0]);
            return -5;
        }
    }
    return n_bytes;
}

static int stk500_paged_load(unsigned int addr, unsigned int n_bytes,
                             unsigned char *data) {
    unsigned char buf[16];
    int a_div;
    int tries;
    unsigned int n;
    int block_size;

    a_div = 2;
    block_size = 128;

    n = addr + n_bytes;
    for (; addr < n; addr += block_size) {
        tries = 0;
    retry:
        tries++;
        stk500_loadaddr(addr / a_div);
        stk500_send(Cmnd_STK_READ_PAGE);
        stk500_send((block_size >> 8) & 0xff);
        stk500_send(block_size & 0xff);
        stk500_send('F');
        stk500_send(Sync_CRC_EOP);

        if (stk500_recv(buf, 1) < 0) return -1;
        if (buf[0] == Resp_STK_NOSYNC) {
            if (tries > 33) {
                debug("stk500_paged_load(): can't get into sync\n");
                return -3;
            }
            if (stk500_getsync() < 0) return -1;
            goto retry;
        } else if (buf[0] != Resp_STK_INSYNC) {
            debug(
                "stk500_paged_load(): (a) protocol error, "
                "expect=0x%02x, resp=0x%02x\n",
                Resp_STK_INSYNC, buf[0]);
            return -4;
        }

        if (stk500_recv(data, block_size) < 0) return -1;
        data += block_size;
        if (stk500_recv(buf, 1) < 0) return -1;

        if (buf[0] != Resp_STK_OK) {
            debug(
                "stk500_paged_load(): (a) protocol error, expect=0x%02x, "
                "resp=0x%02x\n",
                Resp_STK_OK, buf[0]);
            return -5;
        }
    }
    return n_bytes;
}

static int stk500_page_check(int curAddress) {
    int page = address / sizeof(write_page_buff);
    int curpage = curAddress / sizeof(write_page_buff);
    if (page != curpage) {
        if (dity) {
            debug("write page:%04X\n", page * sizeof(write_page_buff));
            int wb =
                stk500_paged_write(page * sizeof(write_page_buff),
                                   sizeof(write_page_buff), write_page_buff);
            if (wb != sizeof(write_page_buff)) return -1;
            int rb = stk500_paged_load(page * sizeof(write_page_buff),
                                       sizeof(read_page_buff), read_page_buff);
            if (rb != sizeof(write_page_buff)) return -1;
            int err = 0;
            for (unsigned int ck = 0; ck < sizeof(write_page_buff); ck++)
                if (read_page_buff[ck] != write_page_buff[ck]) err++;
            if (err) return -1;
        }
        address = curAddress;
        memset(read_page_buff, 0xff, sizeof(read_page_buff));
        memset(write_page_buff, 0xff, sizeof(write_page_buff));
        dity = 0;
    }
    return 0;
}

static int enter_bootloader() {
    Serial.begin(get_settings()->baudrate_isp);
    stk500impl_reset();
    delay(500);
    int i = 0;
    while (1) {
        while (Serial.available()) Serial.read();
        Serial.write(0x30);
        Serial.write(0x20);
        delay(10);
        i++;
        if (i > 500 / 10) {
            return 0;
        }
        if (Serial.available() > 1) {
            if (Serial.read() != 0x14) continue;
            if (Serial.read() != 0x10) continue;
            return 1;
        }
    }
}

static int single_record(uint8_t *data, int len) {
    uint8_t cs = 0;
    uint8_t recsize = (HexToByte(*(data++)) << 4) | HexToByte(*data++);
    cs = recsize;

    if ((recsize * 2 + (1 + 2 + 1 + 1) * 2) != len) return -1;

    uint16_t addr = (HexToByte(*data++) << 4) | HexToByte(*data++);
    cs += (uint8_t)addr;

    addr <<= 8;
    addr |= (HexToByte(*data++) << 4) | HexToByte(*data++);
    cs += (uint8_t)(addr & 0xff);

    uint8_t rectype = (HexToByte(*data++) << 4) | HexToByte(*data++);
    cs += rectype;
    debug("addr:%04X :", addr);
    uint8_t *phexdata = data;
    for (int i = 0; i < recsize; i++) {
        uint8_t d = (HexToByte(*data++) << 4) | HexToByte(*data++);
        cs += d;
        phexdata[i] = d;
    }
    uint8_t csd = (HexToByte(*data++) << 4) | HexToByte(*data++);
    cs += csd;
    if (cs) return -3;

    switch (rectype) {
        case 0:
            for (int i = 0; i < recsize; i++) {
                if (stk500_page_check(baseaddr + addr + i)) return -2;
                write_page_buff[(baseaddr + addr + i) %
                                sizeof(write_page_buff)] = phexdata[i];
            }
            debug("\n");
            if (recsize) dity = 1;
            total_write += recsize;
            break;
        case 1:
            if (stk500_page_check(address + sizeof(write_page_buff))) return -1;
            break;
        case 2:
            baseaddr = (phexdata[0] << 8 | phexdata[1]) << 4;
            return 0;
        case 3:
            return 0;
        case 4:
            baseaddr = (phexdata[0] << 8 | phexdata[1]) << 16;
            return 0;
        case 5:
            return 0;
        default:
            return rectype;
            break;
    }
    return rectype;
}

int handle_hex(WiFiServer *server, WiFiClient *client, int hexlen, int* written) {
    while (client->connected() && client->available() == 0) {
        delay(1);
    }
    if (client->read() != ':') return 0;
    int totalLen = 0;
    address = -1;
    baseaddr = 0;
    dity = 0;
    total_write = 0;
    memset(read_page_buff, 0xff, sizeof(read_page_buff));
    memset(write_page_buff, 0xff, sizeof(write_page_buff));
    if (!enter_bootloader())
        if (!enter_bootloader())
            if (!enter_bootloader()) return -1;
    while (client->connected()) {
        ArduinoOTA.handle();
        if (server->hasClient()) break;
        int rl = client->readBytesUntil(
            ':', lineBuff,
            min((int)sizeof(lineBuff) - 1, hexlen - totalLen - 1));
        int dlen = rl;
        if (dlen < 10) break;
        if (lineBuff[rl - 1] == '\n') dlen--;
        if (lineBuff[rl - 2] == '\r') dlen--;
        if (dlen < 10) break;
        if (dlen & 1) break;
        totalLen += rl + 1;
        if (single_record(lineBuff, dlen)) break;
    }
    *written = total_write;
    return totalLen;
}
