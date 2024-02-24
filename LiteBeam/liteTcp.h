#ifndef __LITEMSG_H__
#define __LITEMSG_H__

#include <Arduino.h>

void init_tcp(byte* buff, unsigned int size);
void tcp_loop();
void tcp_send(const byte *buff, const unsigned int n);

#endif //__LITEMSG_H__