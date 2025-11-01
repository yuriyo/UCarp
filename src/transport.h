#ifndef UCARP_TRANSPORT_H
#define UCARP_TRANSPORT_H 1

#include "ucarp.h"
#include "ip_carp.h"

struct transport_vtable {
	int  (*init)(void);
	int  (*send_advert)(const struct carp_header *ch);
	int  (*poll_recv)(int timeout_ms); /* returns 1 if processed, 0 if timeout, <0 on error */
	void (*teardown)(void);
};

extern const struct transport_vtable *ucarp_transport;
int transport_select(void);

#endif


