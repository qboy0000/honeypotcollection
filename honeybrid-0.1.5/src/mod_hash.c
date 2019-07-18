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

/*! \file mod_hash.c
 * \brief SHA 1 Module for honeybrid Decision Engine
 *
 * This module is called by a boolean decision tree to process a message digest and try to find it in a search table
 *
 *
 \author Julien Vehent, 2007
 \author Thomas Coquelin, 2008
 */

#include "modules.h"

#ifdef HAVE_CRYPTO

#include <ctype.h>
#include <openssl/evp.h>

/*! New process:
 == Initialization ==
 - modules are parsed from the configuration file
 - for each module having the "backup" parameter, a new GKeyFile is initialized (and loaded!), pointer is added to the list of parameters
 - a pointer to the module processing function is added to the list of parameters

 == Processing ==
 - DE find the function to process the packet, then call the function with the following parameters:
 * Pkt structure
 * Module parameter (including GkeyFile to save/load)
 - the module processing function update the decision and the GKeyFile
 - the GkeyFile is added to a backup queue, in order to be written to a file when there is nothing else to do

 ==> Advantages:
 - remove individual initialization functions
 - remove individual *.h files (use only modules.h to put processing function)
 */

/*! \def OpenSSL structure */
const EVP_MD *md;


/*! \brief array indexes of variables to store for each hash 
 port number will be used as separator (group)
 hash will be used as key
 */
#define HASH_COUNTER 0
#define HASH_FIRST_SEEN 1
#define HASH_DURATION 2
#define HASH_PACKET 3
#define HASH_BYTE 4
#define HASH_ASCII 5

int init_mod_hash() {
	g_printerr("%s Initializing Hash Module\n", H(0));

	/*! init OpenSSL SHA-1 engine */
	OpenSSL_add_all_digests();
	md = EVP_get_digestbyname("sha1");

	return 0;
}

/*! mod_hash
 \brief calculate a SHA1 hash value of a packet payload, and look for a possible match in a database of hashes.
 Parameters required:
 function = hash;
 backup	 = /etc/honeybrid/hash.tb
 \param[in] args, struct that contain the node and the datas to process
 \param[in] user_data, not used
 *
 \param[out] set result to 0 if datas's fingerprint is found in search table, 1 if not
 */
void mod_hash(struct mod_args *args) {
	GKeyFile *backup;
	gchar *backup_file;
	int expiration = 24 * 3600;

	/*! First, we make sure that we have data to work on */
	if (args->pkt->data == 0) {
		args->node->result = -1;
		args->node->info_result = -1;
		g_printerr("%s No data to work on\n", H(args->pkt->conn->id));
		return;
	}

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

	uint32_t ascii_len = 64;
	gchar *port;
	gchar **info;

	char *hash;
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len = 20, i = 0;

	char *payload = malloc(args->pkt->data + 1);
	char *ascii;

	gchar **key_dst;

	GTimeVal t;
	g_get_current_time(&t);
	gint now = (t.tv_sec);

	/*! get the IP address from the packet */
	key_dst = g_strsplit(args->pkt->key_dst, ":", 2);

	/*! get the destination port */
	port = key_dst[1];

	/*! get the payload from the packet */
	memcpy(payload, args->pkt->packet.payload, args->pkt->data - 1);
	payload[args->pkt->data] = '\0';

	/*! replace all occurrences of IP addresses by a generic IP */
	GRegex *ip_regex = g_regex_new("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}",
			0, 0, NULL);
	if (TRUE == g_regex_match(ip_regex, payload, 0, NULL)) {
		char *payload_tmp = g_strdup(payload);
		g_printerr("%s found an IP in the payload! Replacing it...\n",
				H(args->pkt->conn->id));
		payload = g_regex_replace(ip_regex, payload_tmp, -1, 0, "<0.0.0.0>", 0,
				NULL);
		g_free(payload_tmp);
	}
	g_regex_unref(ip_regex);

	/* Old method: only taking care of the DST IP address
	 char *position;
	 int j, h, pos;
	 if (strlen(key_dst[0]) >= 7) {
	 while(NULL != (position = strstr(payload, key_dst[0]))) {
	 g_printerr("%s found the dst ip in the payload! Replacing it...\n", H(args->pkt->conn->id));

	 pos = (int)(position-payload);

	 payload[pos+0] = '<';
	 payload[pos+1] = 'A';
	 payload[pos+2] = 'D';
	 payload[pos+3] = 'D';
	 payload[pos+4] = 'R';
	 payload[pos+5] = '>';

	 h=strlen(key_dst[0]) - 6;

	 for(j=(pos+6); j<(strlen(payload)-h); j++) {
	 payload[j] = payload[j+h];
	 }

	 payload[strlen(payload)-h] = '\0';
	 }
	 }
	 */

	if (strlen(payload) < ascii_len) {
		ascii_len = strlen(payload);
	}
	ascii = g_malloc0(ascii_len + 1);

	g_printerr("%s Computing payload digest\n", H(args->pkt->conn->id));

	/*! digest the payload */
	EVP_MD_CTX ctx;
	EVP_MD_CTX_init(&ctx);
	EVP_DigestInit_ex(&ctx, md, NULL);
	EVP_DigestUpdate(&ctx, payload, args->pkt->data - 2);
	EVP_DigestFinal_ex(&ctx, md_value, &md_len);
	EVP_MD_CTX_cleanup(&ctx);

	hash = malloc((md_len << 1) + 1);

	for (i = 0; i < md_len; i++)
		sprintf(hash + (i << 1), "%02x", md_value[i]);

	g_printerr("%s Computing payload ASCII representation\n",
			H(args->pkt->conn->id));

	for (i = 0; i < ascii_len; i++) {
		if (isprint(payload[i])) {
			sprintf(&ascii[i], "%c", payload[i]);
		} else {
			sprintf(&ascii[i], ".");
		}
	}
	ascii[ascii_len] = '\0';

	g_printerr("%s ASCII of %d char [%s]\n", H(args->pkt->conn->id), ascii_len,
			ascii);
	g_printerr("%s Searching for fingerprint in %p on port %s\n",
			H(args->pkt->conn->id), backup, port);

	GError *error = NULL;
	if (NULL
			== (info = g_key_file_get_string_list(backup, port, hash, NULL,
					&error))) {
		/*! Unknown hash, so we accept the packet */
		args->node->result = 1;
		g_printerr("%s Hash not found because: %s\n", H(args->pkt->conn->id),
				error->message);
		g_printerr("%s Packet accepted and new entry created\n",
				H(args->pkt->conn->id));

		info = malloc(6 * sizeof(char *));

		info[HASH_COUNTER] = g_malloc0(20 * sizeof(char));
		info[HASH_FIRST_SEEN] = g_malloc0(20 * sizeof(char));
		info[HASH_DURATION] = g_malloc0(20 * sizeof(char));
		info[HASH_PACKET] = g_malloc0(20 * sizeof(char));
		info[HASH_BYTE] = g_malloc0(20 * sizeof(char));
		info[HASH_ASCII] = g_malloc0(ascii_len * sizeof(char));

		g_snprintf(info[HASH_COUNTER], 20, "1");
		g_snprintf(info[HASH_FIRST_SEEN], 20, "%d", now);
		g_snprintf(info[HASH_DURATION], 20, "0");
		g_snprintf(info[HASH_PACKET], 20, "%d",
				args->pkt->conn->count_data_pkt_from_intruder);
		g_snprintf(info[HASH_BYTE], 20, "%d", args->pkt->data);
		g_snprintf(info[HASH_ASCII], ascii_len, "%s", ascii);

	} else {
		/*! We check if we need to expire this entry */
		int age = atoi(info[HASH_DURATION]);
		if (age > expiration) {
			/*! Known hash but entry expired, so we accept the packet */
			args->node->result = 1;
			g_printerr(
					"%s Hash found but expired... packet accepted and entry renewed\n",
					H(args->pkt->conn->id));

			g_snprintf(info[HASH_COUNTER], 20, "1");
			g_snprintf(info[HASH_FIRST_SEEN], 20, "%d", now);
			g_snprintf(info[HASH_DURATION], 20, "0");

		} else {
			/*! Known hash, so we reject the packet */
			args->node->result = 0;
			g_printerr("%s Hash found... packet rejected and entry updated\n",
					H(args->pkt->conn->id));

			g_snprintf(info[HASH_COUNTER], 20, "%d",
					atoi(info[HASH_COUNTER]) + 1);
			g_snprintf(info[HASH_DURATION], 20, "%d",
					now - atoi(info[HASH_DURATION]));

		}

		g_snprintf(info[HASH_PACKET], 20, "%d",
				args->pkt->conn->count_data_pkt_from_intruder);

	}

	g_printerr("%s Set string list...\n", H(args->pkt->conn->id));

	g_key_file_set_string_list(backup, port, hash, (const gchar * const *) info,
			6);

	g_printerr("%s ... done\n", H(args->pkt->conn->id));

	/*! clean and exit */
	g_free(payload);
	//g_printerr("%s Free 1/5\n", H(args->pkt->conn->id));
	g_free(ascii);
	//g_printerr("%s Free 2/5\n", H(args->pkt->conn->id));
	g_free(hash);
	//g_printerr("%s Free 3/5\n", H(args->pkt->conn->id));
	g_free(port);
	/*
	 g_printerr("%s Free 4/5\n", H(args->pkt->conn->id));
	 g_free(key_dst[0]);
	 g_printerr("%s Free 5/5\n", H(args->pkt->conn->id));
	 g_free(key_dst[1]);
	 g_printerr("%s Free 6/5\n", H(args->pkt->conn->id));
	 g_free(key_dst);
	 g_printerr("%s Free 7/5\n", H(args->pkt->conn->id));
	 */

	save_backup(backup, backup_file);

	return;
}
#endif
