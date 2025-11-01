#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "ucarp.h"
#include "transport.h"

static int raw_init(void) {
	return 0;
}

static int raw_send(const struct carp_header *ch) {
	/* Raw path handled inside carp_send_ad; nothing to do here */
	(void) ch;
	return 0;
}

static int raw_poll(int timeout_ms) {
	(void) timeout_ms;
	return 0;
}

static void raw_teardown(void) {}

static const struct transport_vtable raw_vt = {
	raw_init,
	raw_send,
	raw_poll,
	raw_teardown
};

const struct transport_vtable *ucarp_transport = &raw_vt;

int transport_select(void) {
	if (use_tcp_transport) {
		extern const struct transport_vtable tcp_vt;
		ucarp_transport = &tcp_vt;
		return ucarp_transport->init();
	}
	ucarp_transport = &raw_vt;
	return ucarp_transport->init();
}


