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

#ifndef __CONNECTIONS_H_
#define __CONNECTIONS_H_

#include "types.h"
#include "structs.h"

status_t switch_state(struct conn_struct *conn, int new_state);

status_t init_pkt(unsigned char *nf_packet, struct pkt_struct **pk,
		u_int32_t mark);

void free_pkt(struct pkt_struct *pkt);

status_t store_pkt(struct conn_struct *conn, struct pkt_struct *pkt);

status_t init_conn(struct pkt_struct *pkt, struct conn_struct **conn);

status_t expire_conn(gpointer key, struct conn_struct *cur_conn,
		gint *expiration_delay);

void free_conn(gpointer key, gpointer trash);

status_t init_mark(struct pkt_struct *pkt, const struct conn_struct *conn);

void clean();

status_t setup_redirection(struct conn_struct *conn, uint32_t hih_id);

#endif /* __CONNECTIONS_H_ */
