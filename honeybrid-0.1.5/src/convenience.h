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

#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "types.h"

gpointer config_lookup(const char * parameter, gboolean required);

gint intcmp(gconstpointer a, gconstpointer b, gconstpointer c);

#define ghashtable_foreach(table, i, key, val) \
        g_hash_table_iter_init(i, table); \
        while(g_hash_table_iter_next(i,(void**)key,(void**)val))

#define CONFIG(parameter) \
	(const char *)(config_lookup(parameter, FALSE))

#define CONFIG_REQUIRED(parameter) \
	(const char *)(config_lookup(parameter, TRUE))

#define ICONFIG(parameter) \
	(config_lookup(parameter, FALSE) ? *(const int *)config_lookup(parameter, FALSE) : NOK)

#define ICONFIG_REQUIRED(parameter) \
	*(const int *)config_lookup(parameter, TRUE)

#endif /* __CONFIG_H_ */
