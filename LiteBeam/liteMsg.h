#ifndef __LITEMSG_H__
#define __LITEMSG_H__

#include <Arduino.h>

void init_msg(byte* buff, unsigned int size);
void msg_loop();
void msg_send(const byte *buff, const unsigned int n);

#endif //__LITEMSG_H__