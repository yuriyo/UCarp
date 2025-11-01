#ifndef UCARP_CARP_API_H
#define UCARP_CARP_API_H 1

#include "ucarp.h"

void carp_process_advert_from_ch(const struct carp_header *ch,
				    struct in_addr src_ip);

#endif


