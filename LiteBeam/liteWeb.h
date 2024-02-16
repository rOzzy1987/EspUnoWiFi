#ifndef __LITEWEB_H__
#define __LITEWEB_H__


void init_web(
    void (*reset_slave_fn)(),
    void (*change_ap_fn)(),
    void (*change_st_fn)(),
    void (*update_mdns_fn)());
void web_loop();

#endif // __LITEWEB_H__
