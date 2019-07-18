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

/*! \file mod_source.c
 * \brief Source IP based filtering Module for honeybrid Decision Engine
 *
 * This module is called by a boolean decision tree to filter attacker based on their IP address
 *
 \author Robin Berthier 2009
 */

#include "modules.h"

/*! mod_source
 \brief check if the source IP has already been seen in a prior connection
 Parameters required:
 function = hash;
 backup   = /etc/honeybrid/source.tb
 \param[in] args, struct that contain the node and the data to process
 \param[in] user_data, not used
 *
 \param[out] set result to 0 if attacker ip is found in search table, 1 if not
 */
void mod_source(struct mod_args *args) {
	g_printerr("%s Module called\n", H(args->pkt->conn->id));

	int expiration = 24 * 3600;
	gchar *backup_file;
	gchar **key_src;
	gchar **info;
	GKeyFile *backup;

	GTimeVal t;
	g_get_current_time(&t);
	gint now = (t.tv_sec);

	/*! get the IP address from the packet */
	key_src = g_strsplit(args->pkt->key_src, ":", 0);

	g_printerr("%s source IP is %s\n", H(args->pkt->conn->id), key_src[0]);

	/*! get the backup file for this module */
	if (NULL
			== (backup = (GKeyFile *) g_hash_table_lookup(args->node->arg,
					"backup"))) {
		/*! We can't decide */
		args->node->result = -1;
		g_printerr("%s mandatory argument 'backup' undefined!\n",
				H(args->pkt->conn->id));
		return;
	}
	/*! get the backup file path for this module */
	if (NULL
			== (backup_file = (gchar *) g_hash_table_lookup(args->node->arg,
					"backup_file"))) {
		/*! We can't decide */
		args->node->result = -1;
		g_printerr("%s error, backup file path missing\n",
				H(args->pkt->conn->id));
		return;
	}

	g_printerr("%s searching for this IP in the database...\n",
			H(args->pkt->conn->id));

	if (NULL == (info = g_key_file_get_string_list(backup, "source", /* generic group name \todo: group by port number? */
	key_src[0], NULL, NULL))) {
		/*! Unknown IP, so we accept the packet */
		args->node->result = 1;
		g_printerr("%s IP not found... packet accepted and new entry created\n",
				H(args->pkt->conn->id));

		info = malloc(3 * sizeof(char *));

		/*! 20 characters should be enough to hold even very large numbers */
		info[0] = malloc(20 * sizeof(gchar));
		info[1] = malloc(20 * sizeof(gchar));
		info[2] = malloc(20 * sizeof(gchar));
		g_snprintf(info[0], 20, "1"); /*! counter */
		g_snprintf(info[1], 20, "%d", now); /*! first seen */
		g_snprintf(info[2], 20, "0"); /*! duration */

	} else {
		/*! We check if we need to expire this entry */
		int age = atoi(info[2]);
		if (age > expiration) {
			/*! Known IP but entry expired, so we accept the packet */
			args->node->result = 1;
			g_printerr(
					"%s IP found but expired... packet accepted and entry renewed\n",
					H(args->pkt->conn->id));

			g_snprintf(info[0], 20, "1"); /*! counter */
			g_snprintf(info[1], 20, "%d", now); /*! first seen */
			g_snprintf(info[2], 20, "0"); /*! duration */
		} else {
			/*! Known IP, so we reject the packet */
			args->node->result = 0;
			g_printerr("%s IP found... packet rejected and entry updated\n",
					H(args->pkt->conn->id));

			g_snprintf(info[0], 20, "%d", atoi(info[0]) + 1); /*! counter */
			g_snprintf(info[2], 20, "%d", now - atoi(info[1])); /*! duration */
		}

	}

	g_key_file_set_string_list(backup, "source", key_src[0],
			(const gchar * const *) info, 3);

	save_backup(backup, backup_file);

	/*! clean and exit */
	//g_strfreev(key_src);
	return;
}

