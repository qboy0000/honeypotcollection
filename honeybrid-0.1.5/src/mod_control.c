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

/*! \file mod_control.c
 * \brief Source IP based control engine to rate limit high interaction honeypot
 *
 \author Robin Berthier 2009
 */

#include "modules.h"

/*! control
 \brief calculate the number of packets sent by a same source over a given period of time. If too many packets are sent, following packets are rejected
 Parameters required:
 function = hash;
 backup   = /etc/honeybrid/control.tb
 expiration = 600
 max_packet = 1000
 \param[in] pkts, struct that contain the packet to control
 \param[out] set result to 1 if rate limit reached, 0 otherwise
 */
void mod_control(struct mod_args *args) {
	gchar *backup_file;

	if (args->pkt == NULL) {
		g_printerr("%s Error, NULL packet\n", H(6));
		args->node->result = -1;
		return;
	}

	g_printerr("%s Module called\n", H(args->pkt->conn->id));

	int expiration;
	int max_packet;
	gchar *param;
	gchar **key_src;
	gchar **info;
	GKeyFile *backup;

	GTimeVal t;
	g_get_current_time(&t);
	gint now = (t.tv_sec);

	if (args->pkt->key_src == NULL) {
		g_printerr("%s Error, key_src is NULL\n", H(args->pkt->conn->id));
		args->node->result = -1;
		return;
	}

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

	/*! get control parameters */
	if (NULL
			== (param = (gchar *) g_hash_table_lookup(args->node->arg,
					"expiration"))) {
		/*! no value set for expiration, we go with the default one */
		expiration = 600;
	} else {
		expiration = atoi(param);
	}
	if (NULL
			== (param = (gchar *) g_hash_table_lookup(args->node->arg,
					"max_packet"))) {
		/*! no value set for expiration, we go with the default one */
		max_packet = 1000;
	} else {
		max_packet = atoi(param);
	}

	if (NULL == (info = g_key_file_get_string_list(backup, "source", /* generic group name \todo: group by port number? */
	key_src[0], NULL, NULL))) {
		g_printerr("%s IP not found... new entry created\n",
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
			g_printerr("%s IP found but expired... entry renewed\n",
					H(args->pkt->conn->id));

			g_snprintf(info[0], 20, "1"); /*! counter */
			g_snprintf(info[1], 20, "%d", now); /*! first seen */
			g_snprintf(info[2], 20, "0"); /*! duration */
		} else {
			g_printerr("%s IP found... entry updated\n",
					H(args->pkt->conn->id));

			g_snprintf(info[0], 20, "%d", atoi(info[0]) + 1); /*! counter */
			g_snprintf(info[2], 20, "%d", now - atoi(info[1])); /*! duration */
		}

	}

	if (atoi(info[0]) > max_packet) {
		g_printerr("%s Rate limit reached! Packet rejected\n",
				H(args->pkt->conn->id));
		args->node->result = 0;
	} else {
		g_printerr("%s Rate limit not reached. Packet accepted\n",
				H(args->pkt->conn->id));
		args->node->result = 1;
	}

	g_key_file_set_string_list(backup, "source", key_src[0],
			(const gchar * const *) info, 3);

	save_backup(backup, backup_file);

	/*! clean and exit */
	//g_strfreev(key_src);
	return;
}

