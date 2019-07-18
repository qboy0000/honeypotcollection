/*
 * This file is part of the honeybrid project.
 *
 * Copyright (C) 2007-2009 University of Maryland (http://www.umd.edu)
 * (Written by Robin Berthier <robinb@umd.edu>, Thomas Coquelin <coquelin@umd.edu> and Julien Vehent <julien@linuxwall.info> for the University of Maryland)
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

/*! \file mod_counter.c
 * \brief Packet counter Module for honeybrid Decision Engine
 *
 * This module returns the position of a packet in the connection
 *
 *
 \author Julien Vehent, 2007
 \author Thomas Coquelin, 2008
 */

#include "modules.h"

/*! mod_counter requires the configuration of the following mandatory parameter:
 - "counter", number of packet to receive before accepting
 */

/*! mod_counter
 \param[in] args, struct that contain the node and the datas to process
 \param[out] set result to 1 packet position match arg, 0 otherwise
 */
void mod_counter(struct mod_args *args) {
	int pktval = 0;
	gchar *param;

	g_printerr("%s Module called\n", H(args->pkt->conn->id));

	if ((param = (char *) g_hash_table_lookup(args->node->arg, "counter"))
			== NULL) {
		/*! We can't decide */
		args->node->result = -1;
		g_printerr("%s mandatory argument 'counter' undefined!\n",
				H(args->pkt->conn->id));
		return;
	} else {
		pktval = atoi(param);
	}

	if (pktval <= args->pkt->conn->count_data_pkt_from_intruder) {
		/*! We accept this packet */
		args->node->result = 1;
		g_printerr("%s PACKET MATCH RULE for counter(%d) with value %d\n",
				H(args->pkt->conn->id), pktval,
				args->pkt->conn->count_data_pkt_from_intruder);
	} else {
		/*! We reject this packet */
		args->node->result = 0;
		g_printerr(
				"%s PACKET DOES NOT MATCH RULE for counter(%d) with value %d\n",
				H(args->pkt->conn->id), pktval,
				args->pkt->conn->count_data_pkt_from_intruder);
	}
}

