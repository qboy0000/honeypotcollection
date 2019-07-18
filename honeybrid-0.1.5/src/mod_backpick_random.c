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

/*! \file mod_backpic_random.c
 * \brief BACKPICK RANDOM module for honeybrid Decision Engine
 *
 \author Tamas K Lengyel 2012
 */

#include "modules.h"

/*! mod_backpick_random
 \param[in] args, struct that contain the node and the data to process
 */
void mod_backpick_random(struct mod_args *args) {
	g_printerr("%s Random backpick module called\n", H(args->pkt->conn->id));
	int n_backends = 0;

	if ((n_backends = g_tree_nnodes(args->pkt->conn->target->back_handlers))
			<= 0) {
		g_printerr("%s No backends are defined for this target, rejecting\n",
				H(args->pkt->conn->id));
		args->node->result = 0;
	} else {

		uint32_t pick = rand() % n_backends + 1;

		g_printerr("%s Picking %d out of %d backends\n", H(args->pkt->conn->id),
				pick, n_backends);
		args->backend_use = pick;
		args->node->result = 1;
	}
}

