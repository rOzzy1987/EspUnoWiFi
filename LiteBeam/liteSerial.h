#ifndef __LITESERIAL_H__
#define __LITESERIAL_H__

void init_serial(byte* buff, unsigned int size, void (*callback_fn) (byte* buff, unsigned int n));
void serial_loop();

#endif // __LITESERIAL_H__