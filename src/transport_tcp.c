#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "ucarp.h"
#include "transport.h"
#include "carp_api.h"
#include "log.h"

#include <string.h>
#ifdef HAVE_NETINET_TCP_H
# include <netinet/tcp.h>
#endif

static int listen_fd = -1;
static int conn_fd = -1;

static int tcp_make_socket_nonblock(int fd) {
#ifdef O_NONBLOCK
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	(void) fd; return 0;
#endif
}

static int tcp_connect_peer(void) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) return -1;
	struct sockaddr_in sa; memset(&sa, 0, sizeof sa);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(tcp_port);
	sa.sin_addr = peerip;
    /* Optional low-latency hint */
#ifdef TCP_NODELAY
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (int[]){1}, sizeof(int));
#endif
	connect(fd, (struct sockaddr *)&sa, sizeof sa);
	/* We'll poll later for writability/established; keep it simple: blocking connect */
	return fd;
}

static int tcp_init(void) {
	if (!use_tcp_transport) return 0;
	struct sockaddr_in sa; memset(&sa, 0, sizeof sa);
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		logfile(LOG_ERR, "tcp: socket failed: %s", strerror(errno));
		return -1;
	}
	int one = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(tcp_port);
	sa.sin_addr = srcip; /* bind to the same src ip */
	if (bind(listen_fd, (struct sockaddr *)&sa, sizeof sa) < 0) {
		logfile(LOG_ERR, "tcp: bind failed: %s", strerror(errno));
		return -1;
	}
	if (listen(listen_fd, 1) < 0) {
		logfile(LOG_ERR, "tcp: listen failed: %s", strerror(errno));
		return -1;
	}
	/* Opportunistically establish outgoing connection */
	if (peerip.s_addr != 0) {
		int fd = tcp_connect_peer();
		if (fd >= 0) conn_fd = fd;
	}
	return 0;
}

/* simple frame: 'U','C','A','R', ver=1, vhid, len(2), payload(len) */
static int tcp_send_frame(const unsigned char *buf, size_t len) {
	if (conn_fd < 0) return -1;
	unsigned char hdr[8];
	hdr[0]='U'; hdr[1]='C'; hdr[2]='A'; hdr[3]='R';
	hdr[4]=1; hdr[5]=vhid; hdr[6]=(unsigned char)(len>>8); hdr[7]=(unsigned char)(len&0xff);
	ssize_t n = send(conn_fd, hdr, sizeof hdr, 0);
	if (n != (ssize_t)sizeof hdr) return -1;
	if (len == 0) return 0;
	const unsigned char *p = buf; size_t left = len;
	while (left > 0) {
		ssize_t w = send(conn_fd, p, left, 0);
		if (w <= 0) return -1;
		p += w; left -= (size_t)w;
	}
	return 0;
}

static int tcp_send_advert(const struct carp_header *ch) {
	return tcp_send_frame((const unsigned char *)ch, sizeof *ch);
}

static int tcp_handle_one_frame(void) {
	unsigned char hdr[8];
	ssize_t n = recv(conn_fd, hdr, sizeof hdr, MSG_WAITALL);
	if (n <= 0) return -1;
	if (n != 8 || hdr[0]!='U' || hdr[1]!='C' || hdr[2]!='A' || hdr[3]!='R' || hdr[4]!=1 || hdr[5]!=vhid) {
		return -1;
	}
	unsigned int len = ((unsigned)hdr[6]<<8) | hdr[7];
	if (len != sizeof(struct carp_header)) return -1;
    struct carp_header ch;
	ssize_t m = recv(conn_fd, &ch, sizeof ch, MSG_WAITALL);
	if (m != (ssize_t)sizeof ch) return -1;
    /* Hand off to carp logic (does full validation and state transitions) */
    carp_process_advert_from_ch(&ch, peerip);
	return 1;
}

static int tcp_accept_if_needed(void) {
	if (listen_fd < 0) return 0;
	struct pollfd p = { listen_fd, POLLIN, 0 };
	if (poll(&p, 1, 0) == 1 && (p.revents & POLLIN)) {
		struct sockaddr_in sa; socklen_t sl = sizeof sa;
		int fd = accept(listen_fd, (struct sockaddr *)&sa, &sl);
		if (fd >= 0) {
			if (conn_fd >= 0) close(conn_fd);
			conn_fd = fd;
		}
	}
	return 0;
}

static int tcp_poll(int timeout_ms) {
	(void) timeout_ms;
	tcp_accept_if_needed();
	if (conn_fd >= 0) {
		struct pollfd p = { conn_fd, POLLIN, 0 };
		if (poll(&p, 1, 0) == 1 && (p.revents & POLLIN)) {
			return tcp_handle_one_frame();
		}
	}
	return 0;
}

static void tcp_teardown(void) {
	if (conn_fd >= 0) { close(conn_fd); conn_fd = -1; }
	if (listen_fd >= 0) { close(listen_fd); listen_fd = -1; }
}

const struct transport_vtable tcp_vt = {
	tcp_init,
	tcp_send_advert,
	tcp_poll,
	tcp_teardown
};


