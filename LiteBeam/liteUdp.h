#ifndef __LITEUDP_H__
#define __LITEUDP_H__

#include <Arduino.h>

void init_udp(byte* buff, unsigned int size);

/// @brief Checks if there is any UDP packet in buffer. If there is, its content is forwarded 
/// to serial port, then the client's address is stored for 500ms
void udp_loop();

void udp_send(const byte* buff, const unsigned int n);


#endif // __LITEUDP_H__
