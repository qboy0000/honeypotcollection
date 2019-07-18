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

/*! 
 * \brief XMPP Module for honeybrid Decision Engine
 *
 * This module is called by a boolean decision tree to process a message digest and try to find it in a search table
 *
 \author Tamas K Lengyel, 2012
 */

#include <config.h>

#ifdef HAVE_XMPP
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include <sys/prctl.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "tables.h"
#include "modules.h"
#include "netcode.h"
#include "mod_dionaea.h"

GThread *xmpp_listener_thread, *xmpp_backup_thread;
GTree *dionaea_reference;
GTree *dionaea_connection;
GStaticRWLock dionaea_reference_lock;
GStaticRWLock dionaea_connection_lock;
gchar *xmpp_db;
GKeyFile *xmpp_backup_file;
GList *cleanup;
char *xmpp_fifo;

void InfoDest(void *a) {
	g_printerr("%s Destroying element in dionaea connection tree!\n",H(0));
	free(((struct dionaeaSession*)a)->incident);
	free(((struct dionaeaSession*)a)->localIP);
	free(((struct dionaeaSession*)a)->remoteIP);
	free(((struct dionaeaSession*)a)->transport);
	free((struct dionaeaSession*)a);
}
void InfoDestKeys(void *a) {
	free(((struct dionaeaKeys*)a)->connectionKey);
	free((struct dionaeaKeys*)a);
}

/*
 * Recursive XML parsing, saving dionaea informations to dionaeaEvent structure
 */

static void parseXML(xmlNode *a_node, struct dionaeaEvent *event) {
	xmlNode *cur_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			//g_printerr(" --\n name: %s\n", cur_node->name);
			if( strcmp((char *)cur_node->name, "dionaea")==0 ) {
				event->dionaea=1;
				//g_printerr("Dionaea msg found!\n");
			}
		}

		if(event->dionaea)
		for(xmlAttrPtr attr = cur_node->properties; NULL != attr; attr = attr->next) {
			xmlChar *value=NULL;
			value = xmlGetProp(cur_node,attr->name);
			//g_printerr("  %s: %s\n", attr->name,value);

			if( strcmp((char *)attr->name,"incident")==0 ) {
				event->incident=malloc(50 * sizeof(gchar));
				g_snprintf(event->incident, 50, "%s", value);

				// DIONAEA DOWNLOAD EVENT, THATS GOOD
				if( strncmp(event->incident,"dionaea.download",16)==0 )
				event->download=1;
				else if( strcmp(event->incident,"dionaea.connection.free")==0 || strcmp(event->incident, "dionaea.connection.tcp.reject")==0 )
				event->end=1;
				else if ( strcmp(event->incident,"dionaea.connection.tcp.accept")==0 )
				event->start=1;
			}
			else if( strcmp((char *)attr->name,"ref")==0 || strcmp((char *)attr->name,"child")==0 ) {
				event->reference=atoi((char *)value);
			}
			else if( strcmp((char *)attr->name,"transport")==0 ) {
				event->transport=malloc(4 * sizeof(gchar));
				g_snprintf(event->transport,4, "%s", value);
			}
			else if( strcmp((char *)attr->name, "local_port")==0 ) {
				event->localPort=atoi((char *)value);
			}
			else if( strcmp((char *)attr->name, "local_host")==0 ) {
				event->localIP=malloc(16 * sizeof(gchar));
				g_snprintf(event->localIP,16,"%s",value);
			}
			else if( strcmp((char *)attr->name, "remote_port")==0 ) {
				event->remotePort=atoi((char *)value);
			}
			else if( strcmp((char *)attr->name, "remote_host")==0 ) {
				event->remoteIP=malloc(16 * sizeof(gchar));
				g_snprintf(event->remoteIP,16,"%s",value);
			}
			else if( strcmp((char *)attr->name, "protocol")==0 ) {
				event->protocol=malloc(20 * sizeof(gchar));
				g_snprintf(event->protocol,20,"%s",value);
			}
			xmlFree(value);
		}

		// recursively parse entire XML before going to the next one
		parseXML(cur_node->children, event);
	}
}

void logDionaeaEvent(struct dionaeaEvent *dionaeaEvent) {
	//(char *)g_hash_table_lookup(config,config_ifvar)
}

void handleDionaeaEvent(struct dionaeaEvent *dionaeaEvent) {
	if(dionaeaEvent->dionaea && dionaeaEvent->incident != NULL) {
		g_printerr("%s Dionaea incident: %s, reference: %u\n", H(0),dionaeaEvent->incident, dionaeaEvent->reference);

		struct dionaeaKeys *keys;
		gboolean reference_exists = g_tree_lookup_extended(dionaea_reference, &(dionaeaEvent->reference), NULL, (gpointer *) &keys);

		if (reference_exists == FALSE && dionaeaEvent->start == 1) {
			g_printerr("%s Creating new node in reference GTree with reference key %u!\n", H(0),dionaeaEvent->reference);
			// create new dionaea reference info

			keys=(struct dionaeaKeys *)malloc(sizeof(struct dionaeaKeys));
			keys->reference=dionaeaEvent->reference;
			keys->connectionKey = malloc(100*sizeof(char));
			sprintf(keys->connectionKey, "%s:%s->%s:%u", dionaeaEvent->transport, dionaeaEvent->remoteIP, dionaeaEvent->localIP, dionaeaEvent->localPort);

			unsigned int *reference_key=(unsigned int*)malloc(sizeof(unsigned int));
			*reference_key=dionaeaEvent->reference;
			g_tree_insert(dionaea_reference, reference_key, keys);

		} else if ( reference_exists == FALSE && dionaeaEvent->start == 0 ) {
			// We cought this dionaea message mid-stream, can't handle it! (we don't know which session should we assign it)
			return;
		} else if (reference_exists == TRUE && dionaeaEvent->start ==1) {
			g_printerr("%s Double start of the same dionaea reference?!\n",H(0));
		}

		struct dionaeaSession *session;

		gboolean connection_exists = g_tree_lookup_extended(dionaea_connection, keys->connectionKey, NULL, (gpointer *) &session);
		g_rw_lock_writer_lock (&dionaea_connection_lock);
		if(connection_exists == FALSE) {
			g_printerr("%s Inserting new connection to dionaea conn tree!\n",H(0));
			session=(struct dionaeaSession *)malloc(sizeof(struct dionaeaSession));
			session->download=dionaeaEvent->download;

			GTimeVal t;
			g_get_current_time(&t);
			session->startTime=t.tv_sec;
			session->duration=0;
			session->dlTime=0;

			session->incident=malloc(50*sizeof(gchar));
			g_snprintf(session->incident,50,"%s",dionaeaEvent->incident);

			session->incidentCount=1;
			session->sessionCount=1;
			session->sessionEndCount=0;
			session->localPort=dionaeaEvent->localPort;
			session->remotePort=dionaeaEvent->remotePort;

			session->localIP=malloc(16*sizeof(gchar));
			g_snprintf(session->localIP,16,"%s",dionaeaEvent->localIP);

			session->remoteIP=malloc(16*sizeof(gchar));
			g_snprintf(session->remoteIP,16,"%s",dionaeaEvent->remoteIP);

			session->transport=malloc(4*sizeof(gchar));
			g_snprintf(session->transport,4,"%s",dionaeaEvent->transport);

			char *connkey = malloc(100*sizeof(char));
			g_snprintf(connkey,100,"%s",keys->connectionKey);

			g_tree_insert(dionaea_connection, connkey, session);
		} else {
			GTimeVal t;
			g_get_current_time(&t);

			session->duration=t.tv_sec-session->startTime;

			// only update it if success was found!
			if(session->download != 1 && dionaeaEvent->download == 1) {
				session->download=1;
				session->dlTime=t.tv_sec;

				// Also look up honeybrid conn_tree entry and set dionaea flag!
				char *key0 = malloc(64);
				sprintf(key0, "%s:%i:%s:%i", session->remoteIP, session->remotePort, session->localIP, session->localPort);
				struct conn_struct *conn;

				if (TRUE == g_tree_lookup_extended(conn_tree, key0, NULL,(gpointer *) &conn)) {
					// update stats
					g_rw_lock_writer_lock (&(conn->lock));
					conn->dionaeaDownload=1;
					conn->dionaeaDownloadTime=session->dlTime;
					g_rw_lock_writer_unlock (&(conn->lock));
				} else {

				}
				free(key0);
			}

			g_snprintf(session->incident,50,"%s",dionaeaEvent->incident);

			session->incidentCount++;
			if(dionaeaEvent->start)
			session->sessionCount++;
			if(dionaeaEvent->end)
			session->sessionEndCount++;

			g_printerr("%s Updating dionaea conn tree entry! i:%u a:%u e:%u\n",H(0),session->incidentCount,session->sessionCount,session->sessionEndCount);
		}
		g_rw_lock_writer_unlock (&dionaea_connection_lock);

		if(dionaeaEvent->end) {
			g_printerr("%s Removing reference key %u!\n", H(0),dionaeaEvent->reference);
			unsigned int *refkey=(unsigned int*)malloc(sizeof(unsigned int));
			*refkey=dionaeaEvent->reference;
			g_tree_remove(dionaea_reference,refkey);
		}
	}
}

/*
 * XMPP listener, keeps querying local FIFO queue for new XML messages
 * Once the XML message is parsed, the corresponding Dionaea session is looked-up
 * in the GTree structure and updated accordingly.
 */

void xmpp_listener() {

	xmlDocPtr doc = NULL; /* the resulting document tree */
	while(1) {
		doc = xmlReadFile(xmpp_fifo, NULL, XML_PARSE_RECOVER);
		if (doc == NULL) {
			g_printerr("%s Failed to parse XML from FIFO queue!\n",H(0));
		} else {
			//g_printerr("--------------- Got a message!\n");
			struct dionaeaEvent * dionaeaEvent=(struct dionaeaEvent *)malloc(sizeof(struct dionaeaEvent));
			dionaeaEvent->download=0;
			dionaeaEvent->end=0;
			dionaeaEvent->start=0;
			dionaeaEvent->dionaea=0;
			dionaeaEvent->localPort=0;
			dionaeaEvent->remotePort=0;
			dionaeaEvent->reference=0;
			dionaeaEvent->incident=NULL;
			dionaeaEvent->localIP=NULL;
			dionaeaEvent->remoteIP=NULL;
			dionaeaEvent->transport=NULL;
			dionaeaEvent->protocol=NULL;

			xmlNode *root_element = xmlDocGetRootElement(doc);
			parseXML(root_element,dionaeaEvent);
			handleDionaeaEvent(dionaeaEvent);

			g_free(dionaeaEvent->incident);
			g_free(dionaeaEvent->localIP);
			g_free(dionaeaEvent->remoteIP);
			g_free(dionaeaEvent->transport);
			g_free(dionaeaEvent->protocol);
			free(dionaeaEvent);

		}
		xmlFreeDoc(doc);
		xmlCleanupParser();

	}
	xmlMemoryDump();
}

/*
 * XMPP Backup thread, loops connection tree and removes all finished dionaea sessions and save it to backup
 */
gboolean xmpp_loop_tree(gpointer key, gpointer value, gpointer data) {
	struct dionaeaSession *session=(struct dionaeaSession *)value;
	gchar **info;

	if(session->sessionCount==session->sessionEndCount) {
		if ( NULL == (info = g_key_file_get_string_list(
								xmpp_backup_file,
								"dionaea",
								(gchar *)key,
								NULL,
								NULL)
				))
		{
			info=malloc(6 * sizeof(gchar*));
			info[0]=malloc(2*sizeof(gchar)); // dionaea success/failure
			g_snprintf(info[0],2,"%u",session->download);
			info[1]=malloc(20*sizeof(gchar));// first seen
			g_snprintf(info[1],20,"%i",session->startTime);
			info[2]=malloc(20*sizeof(gchar));// last seen
			g_snprintf(info[2],20,"%i",session->startTime+session->duration);
			info[3]=malloc(20*sizeof(gchar));
			g_snprintf(info[3],20,"%i",session->dlTime);
			info[4]=malloc(20*sizeof(gchar));
			g_snprintf(info[4],20,"%u",session->sessionCount);
			info[5]=malloc(20*sizeof(gchar));
			g_snprintf(info[5],20,"%u",session->incidentCount);

		} else {
			if(atoi(info[0])!=1 && session->download==1) {
				g_snprintf(info[0],2,"%u",session->download);
				g_snprintf(info[3],20,"%i",session->dlTime);
			}

			g_snprintf(info[2],20,"%i",session->startTime+session->duration);
			g_snprintf(info[4],20,"%u",atoi(info[4])+session->sessionCount);
			g_snprintf(info[5],20,"%u",atoi(info[5])+session->incidentCount);
		}

		g_key_file_set_string_list(
				xmpp_backup_file,
				"dionaea",
				(gchar *)key,
				(const gchar * const *)info,
				6
		);

		//g_printerr("%s XMPP Backup has added an entry: %s\n", H(0), (gchar *)key);
		save_backup(xmpp_backup_file, xmpp_db);
		cleanup=g_list_prepend(cleanup,key);
	}
	return FALSE;
}
void xmpp_backup() {
	// Loop the conn tree every minute and save sessions that are all closed!
	while(1) {
		//g_printerr("XMPP Backup running\n");
		cleanup=NULL;
		g_rw_lock_writer_lock (&dionaea_connection_lock);
		g_tree_foreach(dionaea_connection, xmpp_loop_tree, NULL);
		int counter=0;

		while(cleanup!=NULL) {
			gchar *key = (gchar *)cleanup->data;
			//g_printerr("%s Removing key from dionaea conn tree: %s\n",H(0),key);
			g_tree_remove(dionaea_connection,key);
			counter++;
			cleanup=g_list_next(cleanup);
		}

		g_printerr("%s Removed %i elements from dionaea conn tree!\n", H(0),counter);
		g_list_free(cleanup);
		g_rw_lock_writer_unlock (&dionaea_connection_lock);
		g_usleep(60000000);
	}
}

/*
 * Since the mod_dionaea() function is event-driver, we need to create our own garbage-collection
 * otherwise the dionaea connection tree will hold a lot of connections for no reason.
 */
int init_mod_dionaea()
{
	/* Check if it is actually going to be used, otherwise don't start XMPP listener */

	gchar *listener=NULL;
	gchar *xmpp_debug=NULL;
	gchar *xmpp_user=NULL;
	gchar *xmpp_server=NULL;
	gchar *xmpp_password=NULL;
	gchar *xmpp_channel=NULL;
	gchar *xmpp_domain=NULL;
	xmpp_fifo=NULL;

	if(NULL == (listener=(gchar *)g_hash_table_lookup(config,"xmpp_listener"))) {
		// We assume that if no listener is defined than dionaea module is not used!
		// Skipping thread creations and XMPP listener start
		return 0;
	} else {
		FILE * file = NULL;
		if (NULL != (file = fopen(listener, "r")))
		{
			fclose(file);
		} else {
			errx(1,"%s: XMPP listener doesn't exist!\n",__func__);
		}
	}

	if(NULL == (xmpp_debug=(gchar *)g_hash_table_lookup(config,"xmpp_debug"))) {
		xmpp_debug="/dev/null";
	}

	if(NULL == (xmpp_user=(gchar *)g_hash_table_lookup(config,"xmpp_user"))) {
		errx(1,"%s: XMPP user not defined!\n",__func__);
	}

	if(NULL == (xmpp_server=(gchar *)g_hash_table_lookup(config,"xmpp_server"))) {
		errx(1,"%s: XMPP server not defined!\n",__func__);
	}

	if(NULL == (xmpp_password=(gchar *)g_hash_table_lookup(config,"xmpp_password"))) {
		errx(1,"%s: XMPP password not defined!\n",__func__);
	}

	if(NULL == (xmpp_channel=(gchar *)g_hash_table_lookup(config,"xmpp_channel"))) {
		errx(1,"%s: XMPP channel not defined!\n",__func__);
	}

	if(NULL == (xmpp_domain=(gchar *)g_hash_table_lookup(config,"xmpp_domain"))) {
		errx(1,"%s: XMPP domain not defined!\n",__func__);
	}

	if(NULL == (xmpp_fifo=(gchar *)g_hash_table_lookup(config,"xmpp_fifo"))) {
		errx(1,"%s: XMPP FIFO file not defined!\n",__func__);
	}

	int abyss = open(xmpp_debug, O_WRONLY);
	pid_t pID = fork();
	if (pID == 0) {
		// Code only executed by child process
		dup2(abyss, STDOUT_FILENO);
		dup2(abyss, STDERR_FILENO);
		prctl(PR_SET_PDEATHSIG, SIGHUP);// pass sighup to die if parent dies

		execl(listener,
				xmpp_server,
				xmpp_user,
				xmpp_password,
				xmpp_channel,
				xmpp_domain,
				xmpp_fifo,
				(char *)NULL);

	} else if (pID < 0) {
		errx(1,"Failed to fork!\n");
	}

	g_rw_lock_init( &dionaea_reference_lock );
	g_rw_lock_init( &dionaea_connection_lock );

	if(NULL != (xmpp_db=(gchar *)g_hash_table_lookup(config,"xmpp_db"))) {
		xmpp_backup_file=g_key_file_new();
		if(g_key_file_load_from_file(xmpp_backup_file, xmpp_db,0,NULL)) {
			g_printerr("%s Dionaea XMPP backup loaded from file %s\n",H(0),xmpp_db);
		}
	} else {
		errx(1,"%s: Dionaea XMPP backup file not defined!\n",__func__);
	}

	if (NULL == (dionaea_reference =
					g_tree_new_full((GCompareDataFunc)IntComp,NULL,(GDestroyNotify)IntDest,(GDestroyNotify)InfoDestKeys))
	) {
		errx(1, "%s: Fatal error while creating dionaea_reference GTree.\n", __func__);
	}

	if (NULL == (dionaea_connection =
					g_tree_new_full((GCompareDataFunc)strcmp,NULL,(GDestroyNotify)g_free,(GDestroyNotify)InfoDest))
	) {
		errx(1, "%s: Fatal error while creating dionaea_connection GTree.\n", __func__);
	}

	// create xmpp xml reader thread
	if( ( xmpp_listener_thread = g_thread_new ("xmpp_listener", ((void *)xmpp_listener), NULL)) == NULL)
	errx(1, "%s: Unable to start the XMPP listener thread", __func__);
	else
	g_printerr("%s: Dionaea XMPP listener thread started\n", H(0));

	// create xmpp backup thread
	if( ( xmpp_backup_thread = g_thread_new ("xmpp_backup", ((void *)xmpp_backup), NULL)) == NULL)
	errx(1, "%s: Unable to start the XMPP backup thread", __func__);
	else
	g_printerr("%s: Dionaea XMPP backup thread started\n", H(0));

	return 0;
}

void mod_dionaea(struct mod_args *args)
{

	// Check if module is configured properly
	if(xmpp_fifo==NULL) {
		// It's not, can't decide
		g_printerr("%s Dionaea module is not configured properly, can't decide!!\n", H(args->pkt->conn->id));
		args->node->result = -1;
		return;
	}

	//! get the IP address from the packet
	gchar **key_src = g_strsplit( args->pkt->key_src, ":", 0 );
	gchar **key_dst = g_strsplit(args->pkt->key_dst, ":", 2);
	//! get the destination port
	gchar *srcport = key_src[1];
	gchar *dstport = key_dst[1];
	gchar **info;

	//int expiration = 24*3600;
	//GTimeVal t;
	//g_get_current_time(&t);
	//gint now = (t.tv_sec);

	u_short protocol = args->pkt->packet.ip->protocol;

	g_printerr("%s: Protocol: %u | %i | %s:%s -> %s:%s | M:%u\n", H(args->pkt->conn->id), protocol, args->pkt->origin, key_src[0], srcport, key_dst[0], dstport,args->pkt->conn->mark);

	gchar *mode;
	/*! get the backup file for this module */
	if ( NULL == (mode = (gchar *)g_hash_table_lookup(args->node->arg, "mode"))) {
		/*! We can't decide */
		args->node->result = -1;
		g_printerr("%s mandatory argument 'mode' undefined (front/back)!\n", H(args->pkt->conn->id));
		return;
	}

	gchar *protocolString;
	if(protocol==6)
	protocolString="tcp";
	else if(protocol==17)
	protocolString="udp";

	char *connection_key = malloc(70);
	sprintf(connection_key, "%s:%s->%s:%s", protocolString, key_src[0], key_dst[0], dstport);

	// Check the backup for info
	int dionaeaBackupSuccess=-1;
	if ( NULL != (info = g_key_file_get_string_list(
							xmpp_backup_file,
							"dionaea",
							connection_key,
							NULL,
							NULL)
			)
	) {
		g_printerr("%s XMPP Backup info found: %s\n", H(args->pkt->conn->id),info[0]);
		if( strcmp(info[0],"1")==0 ) {
			// According to backup dionaea handled this in the past successfully
			dionaeaBackupSuccess=1;
		} else {
			// According to backup dionaea didn't handle this in the past successfully
			dionaeaBackupSuccess=0;
		}
	}

	//Let's also check the BTree for info
	struct dionaeaSession *session;
	int dionaeaBTreeSuccess=-1;
	if (TRUE == g_tree_lookup_extended(dionaea_connection, connection_key, NULL, (gpointer *) &session)) {
		if(session->download) {
			// Dionaea handled it recently with success
			dionaeaBTreeSuccess=1;
		} else {
			// Dionaea handled it recently and failed!
			dionaeaBTreeSuccess=0;
		}
	}

	/*if(dionaeaBTreeSuccess==1 || dionaeaBackupSuccess!=0)
	 args->node->result=0;
	 else
	 if(dionaeaBackupSuccess==0 && now-atoi(info[2])<expiration)
	 args->node->result=1;
	 else
	 args->node->result=0;*/

	if(strcmp(mode,"front") == 0) {

		// Only allow it till we get a payload
		if(dionaeaBackupSuccess==1 || dionaeaBTreeSuccess==1)
		args->node->result=0;
		else
		args->node->result=1;

	} else {
		// Only switch it if we didn't get a payload
		if(dionaeaBackupSuccess==1 || dionaeaBTreeSuccess==1 || ( dionaeaBackupSuccess==-1 && dionaeaBTreeSuccess==-1 ) )
		args->node->result=0;
		else
		args->node->result=1;
	}

	free(connection_key);
	free(info);

	return;
}
#endif

