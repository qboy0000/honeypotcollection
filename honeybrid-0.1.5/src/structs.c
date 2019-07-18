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

#include "structs.h"
#include "decision_engine.h"

/*!	\file structs.c
 \brief

 This file is intended to provide a place for the struct free functions
 to be placed at. It is not a requirement, as sometimes it makes the code
 easier to read to have the free function next to it's init counterpart.

 */

void free_interface(struct interface *iface) {
	if (iface) {
		g_free(iface->ip);
		g_free(iface->ip_str);
		g_free(iface->name);
		g_free(iface->tag);
		g_free(iface);
	}
}

void free_backend(struct backend *backend) {
	if (backend) {
		DE_destroy_tree(backend->rule);
		free_interface(backend->iface);
		g_free(backend);
	}
}

void free_target_gfunc(struct target *t, gpointer user_data) {
	g_free(t->filter);
	g_free(t->front_handler);
	g_tree_destroy(t->back_handlers);
	g_hash_table_destroy(t->unique_backend_ips);
	DE_destroy_tree(t->front_rule);
	DE_destroy_tree(t->control_rule);
	g_free(t);
}
