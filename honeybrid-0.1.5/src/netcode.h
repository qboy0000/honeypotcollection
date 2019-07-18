/*
 * This file is part of the honeybrid project.
 *
 * 2007-2009 University of Maryland (http://www.umd.edu)
 * (Written by Robin Berthier <robinb@umd.edu>, Thomas Coquelin <coquelin@umd.edu> and Julien Vehent <julien@linuxwall.info> for the University of Maryland)
 *
 * 2012-2013 University of Connecticut (http://www.uconn.edu)
 * (Extended by Tamas K Lengyel <tamas.k.lengyel@gmail.com>
 *
 * Honeybrid is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NETCODE_H_
#define _NETCODE_H_

#include "types.h"
#include "structs.h"

#define ip_checksum(hdr) \
	((struct iphdr*)hdr)->check = \
		in_cksum(hdr, \
		sizeof(struct iphdr))

#define udp_checksum(hdr) \
	((struct udp_packet *)hdr)->udp.check = \
		in_cksum( hdr, \
        sizeof(struct udphdr))

struct interface *uplinks;

status_t send_raw(const struct iphdr *p, const struct interface *iface);

status_t forward(struct pkt_struct* pkt);

status_t reply_reset(const struct packet *p);

status_t reset_lih(struct conn_struct* connection_data);

status_t replay(struct conn_struct* connection_data, struct pkt_struct* pkt);

void tcp_checksum(struct tcp_packet* pkt);

status_t define_expected_data(struct pkt_struct* pkt);

status_t test_expected(struct conn_struct* connection_data, struct pkt_struct* pkt);

status_t init_raw_sockets();

void init_raw_sockets_backends(gpointer target, gpointer extra);

gboolean init_raw_sockets_backends2(gpointer key, gpointer value, gpointer extra);

int addr2int(const char *address);

#endif // _NETCODE_H_
