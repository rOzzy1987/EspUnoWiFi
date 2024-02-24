#ifndef __LITEWEB_H__
#define __LITEWEB_H__


void init_web(void (*reset_slave_fn)());
void web_loop(bool is_captive = false);

#endif // __LITEWEB_H__
