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

/*!	\mainpage Hybrid Honeypot Gateway
 *
 * 	\section Introduction
 *
 * 	This project is a honeypot architecture able to combine low and high interaction honeypots in the same framework.
 * 	The objective is to have a scalable solutions and to collect detailed attack processes.
 * 	The core of the project is a software gateway based on Iptables and built upon two engines: a Redirection Engine and a Decision Engine.
 * 	The concept of the architecture is to use a front end of low interaction honeypot to reply to all incoming traffic.
 * 	Then the goal of the Decision Engine is to filter interesting attacks from the noise of incoming traffic received.
 * 	Filtered attacks are forwarded to the Redirection Engine which is able to actively redirect the destination of the connection, so that it can be further investigated using a high interaction back-end.
 *
 * 	\section Requirements
 *
 * 	Dependencies:
 *	- linux kernel >= 2.6.18 & <=2.6.23
 * 	- libnetfilter-queue-dev & libnetfilter-queue1
 *	- libnfnetlink >= 0.0.25
 *	- libglib2.0-dev & libglib2.0-0
 *	- openssl
 *	- libssl-dev
 *	- libev
 *
 * 	\section Installation
 *
 *	Installation is defined in the INSTALL file.
 *
 */

/*!	\file honeybrid.c
 \brief Main File

 This is the main program file for Honeybrid. It creates a hook using LibNetfilter Queue
 and, for each connection, maintain a stateful table.
 It forward a packet to a determined destination and submit this packet to the decision engine.
 When the decision engine decide to redirect a connection, this redirection engine replay the recorded
 connection to its new destination and maintain it until its end.

 Packets needs to be redirected to the QUEUE destination using netfilter, this can be done using:
 # iptables -A INPUT -j QUEUE && iptables -A FORWARD -j QUEUE && iptables -A OUTPUT -j QUEUE

 filters can also be set up using the regulars iptables capabilities, it is also recommended to limit the redirections to TCP and UDP packets (just add the option -p to the iptables commands)

 Dependencies:
 - libnetfilter-queue-dev & libnetfilter-queue1 >= 1.0
 - libnfnetlink >= 0.0.25
 - libglib2.0-dev & libglib2.0-0 >= 2.32

 \Author J. Vehent, 2007
 \Author Thomas Coquelin, 2008
 \Author Robin Berthier, 2007-2009
 \Author Tamas K Lengyel, 2012-2013
 */

#include "honeybrid.h"

#include <limits.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <sys/stat.h>
#include <execinfo.h>

#include "constants.h"
#include "structs.h"
#include "globals.h"
#include "convenience.h"
#include "netcode.h"
#include "log.h"
#include "decision_engine.h"
#include "modules.h"
#include "connections.h"

#ifdef HAVE_LIBEV
#include <ev.h>
struct nfq_handle *h;
struct ev_loop *loop;
#endif

/*! usage function
 \brief print command line informations */
void usage(char **argv) {
	g_printerr(
			"Honeybrid version %s\n"
					"usage: %s <commands>\n\n"
					"where commands include:\n"
					"  -c <config_file>: start with config file\n"
					"  -x <pid>: halt the engine using its PID\n"
					"  -q <queuenum>: select a specific queue number for NF_QUEUE to listen to\n"
					//"  -s: show status information\n"
					"  -h: print the help\n\n", PACKAGE_VERSION, argv[0]);
	exit(1);
}

/*! term_signal_handler
 *
 \brief called when the program receive a signal that should close the program, free memory and delete lock file
 *
 \param[in] signal_nb: number of the signal
 \param[in] siginfo: informations regarding to the signal
 \param[in] context: NULL */
int term_signal_handler(int signal_nb, siginfo_t * siginfo, void *context) {
	g_printerr("%s: Signal %d received, halting engine\n", __func__, signal_nb);
#ifdef DEBUG
	g_printerr("* Signal number:\t%d\n", siginfo->si_signo);
	g_printerr("* Signal code:  \t%d\n", siginfo->si_code);
	g_printerr("* Signal error: \t%d '%s'\n", siginfo->si_errno,
			strerror(siginfo->si_errno));
	g_printerr("* Sending pid:  \t%d\n", siginfo->si_pid);
	g_printerr("* Sending uid:  \t%d\n", siginfo->si_uid);
	g_printerr("* Fault address:\t%p\n", siginfo->si_addr);
	g_printerr("* Exit value:   \t%d\n", siginfo->si_status);
#endif
	running = NOK; /*! this will cause the queue loop to stop */

#ifdef HAVE_LIBEV
	ev_unloop (loop, EVUNLOOP_ALL);
#endif
	return 0;
}

/*! init_signal
 \brief installs signal handlers
 \return 0 if exit with success, anything else if not */
void init_signal() {
	/*! Install terminating signal handler: */
	struct sigaction sa_term;
	memset(&sa_term, 0, sizeof sa_term);

	sa_term.sa_sigaction = (void *) term_signal_handler;
	sa_term.sa_flags = SA_SIGINFO | SA_RESETHAND;
	sigfillset(&sa_term.sa_mask);

	/*! SIGHUP*/
	if (sigaction(SIGHUP, &sa_term, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGHUP", __func__);

	/*! SIGINT*/
	if (sigaction(SIGINT, &sa_term, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGINT", __func__);

	/*! SIGQUIT*/
	if (sigaction(SIGQUIT, &sa_term, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGQUIT", __func__);

	/*! SIGILL*/
	if (sigaction(SIGILL, &sa_term, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGILL", __func__);

	/*! SIGSEGV*/
	if (sigaction(SIGSEGV, &sa_term, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGSEGV", __func__);

	/*! SIGTERM*/
	if (sigaction(SIGTERM, &sa_term, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGTERM", __func__);

	/*! SIGBUS*/
	if (sigaction(SIGBUS, &sa_term, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGBUS", __func__);

	/*! ignore signals: */
	struct sigaction sa_ignore;
	memset(&sa_ignore, 0, sizeof sa_ignore);
	sa_ignore.sa_handler = SIG_IGN;
	sigfillset(&sa_ignore.sa_mask);

	/*! SIGABRT*/
	if (sigaction(SIGABRT, &sa_ignore, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGABRT", __func__);

	/*! SIGALRM*/
	if (sigaction(SIGALRM, &sa_ignore, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGALRM", __func__);

	/*! SIGUSR2*/
	if (sigaction(SIGUSR2, &sa_ignore, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGUSR2", __func__);

	/*! SIGPOLL*/
	if (sigaction(SIGPOLL, &sa_ignore, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGPOLL", __func__);

	/*! rotate logs: */
	struct sigaction sa_rotate_log;
	memset(&sa_rotate_log, 0, sizeof sa_rotate_log);

	sa_rotate_log.sa_sigaction = (void *) rotate_connection_log;
	//sa_rotate_log.sa_flags = SA_SIGINFO | SA_RESETHAND;
	sa_rotate_log.sa_flags = SA_RESTART;
	sigfillset(&sa_rotate_log.sa_mask);

	/*! SIGUSR1*/
	if (sigaction(SIGUSR1, &sa_rotate_log, NULL) != 0)
		errx(1, "%s: Failed to install sighandler for SIGUSR1", __func__);
}

/*! init_syslog
 \brief initialize syslog logging */
static void init_syslog(int argc, char *argv[]) {
	int options, i;
	char buf[MAXPATHLEN];

#ifdef LOG_PERROR
	options = LOG_PERROR | LOG_PID | LOG_CONS;
#else
	options = LOG_PID|LOG_CONS;
#endif
	openlog("honeybrid", options, LOG_DAEMON);

	/* Create a string containing all the command line
	 * arguments and pass it to syslog:
	 */

	buf[0] = '\0';
	for (i = 1; i < argc; i++) {
		if (i > 1 && g_strlcat(buf, " ", sizeof(buf)) >= sizeof(buf))
			break;
		if (g_strlcat(buf, argv[i], sizeof(buf)) >= sizeof(buf))
			break;
	}

	syslog(LOG_NOTICE, "started with %s", buf);
}

/*! parse_config
 \brief Configuration parsing function, read the configuration from a specific file 
 and parse it into a hash table or other tree data structures using Bison/Flex
 */
void init_parser(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (!fp)
		err(1, "fopen(%s)", filename);

	//extern int yydebug;
	//yydebug = 1;
	yyin = fp;
	yyparse();

	fclose(fp);

	g_printerr("Parsing done\n");
}

void init_variables() {
	/*! create the hash table to store the config */
	if (NULL
			== (config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
					g_free)))
		errx(1, "%s: Fatal error while creating config hash table.\n",
				__func__);

	/*! create the array of pointer to store the target information */
	if (NULL == (targets = g_ptr_array_new()))
		errx(1, "%s: Fatal error while creating targets pointer of array.\n",
				__func__);

	/*! create the hash table to store module information */
	if (NULL
			== (module = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
					(GDestroyNotify) g_hash_table_destroy)))
		errx(1, "%s: Fatal error while creating module hash table.\n",
				__func__);

	/*! create the hash table for the log engine */
	if (NULL
			== (uplink = g_hash_table_new_full(g_int_hash, g_int_equal, NULL,
					(GDestroyNotify) free_interface)))
		errx(1, "%s: Fatal error while creating uplink hash table.\n",
				__func__);

#ifdef DE_THREAD
	/*! init DE_queue */
	DE_queue = NULL;
#endif

	/*! init the security locks */
#ifdef DE_THREAD
	g_rw_lock_init( &DE_queue_lock );
#endif

	/* create the main B-Tree to store meta informations of active connections */
	if (NULL == (conn_tree = g_tree_new((GCompareFunc) g_strcmp0))) {
		errx(1, "%s: Fatal error while creating conn_tree.\n", __func__);
	}

	/*! create the hash table for the log engine */
	if (NULL == (module_to_save = g_hash_table_new(g_str_hash, g_str_equal)))
		errx(1, "%s: Fatal error while creating module_to_save hash table.\n",
				__func__);

	/*! create the redirection table */
	if (NULL
			== (high_redirection_table = g_hash_table_new_full(g_str_hash,
					g_str_equal, g_free, g_free)))
		errx(1,
				"%s: Fatal error while creating high_redirection_table hash table.\n",
				__func__);

	/*! init the connection id counter */
	c_id = 1;

	/*! Enable threads */
	threading = OK;

	/*! Enable data processing */
	running = OK;
}

/*! close_thread
 \brief Function that waits for thread to close themselves */
int close_thread() {

	threading = NOK;
	g_cond_broadcast(&threading_cond);

#ifndef HAVE_LIBEV
	g_printerr("%s: Waiting for thread_clean to terminate\n", __func__);
	g_thread_join(thread_clean);
#endif

#ifdef DE_THREAD
	g_printerr("%s: Waiting for thread_de to terminate\n", __func__);
	g_thread_join(thread_de);
#endif

	g_thread_join(mod_backup);

	return 0;
}

/*! close_hash function
 \brief Destroy the different hashes used by honeybrid */
int close_hash() {
	/*! Destroy hash tables
	 */

	if (high_redirection_table != NULL) {
		g_printerr("%s: Destroying table high_redirection_table\n", __func__);
		g_rw_lock_writer_lock(&hihredirlock);
		g_hash_table_destroy(high_redirection_table);
		high_redirection_table = NULL;
	}

	if (config != NULL) {
		g_printerr("%s: Destroying table config\n", __func__);
		g_hash_table_destroy(config);
		config = NULL;
	}

	if (module != NULL) {
		g_printerr("%s: Destroying table module\n", __func__);
		g_hash_table_destroy(module);
	}

	if (uplink != NULL) {
		g_printerr("%s: Destroying table uplink\n", __func__);
		g_hash_table_destroy(uplink);
		uplink = NULL;
	}

	if (module_to_save != NULL) {
		g_printerr("%s: Destroying table module_to_save\n", __func__);
		g_hash_table_destroy(module_to_save);
		module_to_save = NULL;
	}

	return 0;
}

/*! close_conn_tree function
 \brief Function to free memory taken by conn_tree */
int close_conn_tree() {

	g_printerr("%s: Destroying connection tree\n", __func__);

	/*! clean the memory
	 * traverse the B-Tree to remove the singly linked lists and then destroy the B-Tree
	 */
	int delay = 0;
	entrytoclean = g_ptr_array_new();

	/*! call the clean function for each value, delete the value if TRUE is returned */
	g_tree_foreach(conn_tree, (GTraverseFunc) expire_conn, &delay);

	/*! remove each key listed from the btree */
	g_ptr_array_foreach(entrytoclean, (GFunc) free_conn, NULL);

	/*! free the array */
	g_ptr_array_free(entrytoclean, TRUE);
	entrytoclean = NULL;

	g_tree_destroy(conn_tree);
	conn_tree = NULL;

	return 0;
}

/*! close_target
 \brief destroy global structure "targets" when the program has to quit */
int close_target(void) {
	g_printerr("%s: Destroying targets\n", __func__);
	g_ptr_array_foreach(targets, (GFunc) free_target_gfunc, NULL);
	g_ptr_array_free(targets, TRUE);
	return OK;
}

/*! close_all
 \brief destroy structures and free memory when the program has to quit */
void close_all(void) {
	/*! wait for thread to close */
	if (close_thread() < 0)
		g_printerr("%s: Error when waiting for threads to close\n", __func__);

	/*! delete conn_tree */
	if (close_conn_tree() < 0)
		g_printerr("%s: Error when closing conn_tree\n", __func__);

	/*! delete lock file (only if the process ran as a daemon) */
	output_t output = ICONFIG_REQUIRED("output");
	if (output != OUTPUT_STDOUT) {
		if (unlink(pidfile) < 0)
			g_printerr("%s: Error when removing lock file\n", __func__);
	}

	/*! close log file */
	if (output == OUTPUT_LOGFILES) {
		close_connection_log();
	}

	/*! delete hashes */
	if (close_hash() < 0)
		g_printerr("%s: Error when closing hashes\n", __func__);

	if (close_target() < 0)
		g_printerr("%s: Error when closing targets\n", __func__);

}

/*! process_packet
 *
 \brief Function called for each received packet, this is the core of the redirection engine
 \param[in] tb a Netfilter Queue structure that contain both the packet and the metadatas
 \param[in] *mark a pointer to the mark field to be set on the packet
 \return statement = 1 if the packet should be accepted or 0 if the packet has to be dropped. Default is to drop. */
struct verdict *
process_packet(struct nfq_data *tb) {

	/*! We create the verdict structure to return */
	struct verdict *to_return = g_malloc0(sizeof(struct verdict));

	struct conn_struct * conn = NULL;
	struct pkt_struct * pkt = NULL;

	unsigned char *nf_packet;
	struct in_addr in;

	/*! extract ip header from packet payload */
	int size;
	size = nfq_get_payload(tb, &nf_packet);
	if (size < 0) {
		return to_return;
	}

	in.s_addr = ((struct iphdr*) nf_packet)->saddr;

	g_printerr("%s** NEW packet from %s %s, %d bytes. Mark %u **\n", H(0),
			inet_ntoa(in), lookup_proto(((struct iphdr*) nf_packet)->protocol),
			size, to_return->mark);

	/*! check if protocol is invalid (not TCP or UDP) */
	if ((((struct iphdr*) nf_packet)->protocol != TCP)
			&& (((struct iphdr*) nf_packet)->protocol != UDP)) {
		g_printerr("%s Incorrect protocol: %d, packet dropped\n", H(0),
				(((struct iphdr*) nf_packet)->protocol));
		return to_return;
	}

	/*! Initialize the packet structure (into pkt) and find the origin of the packet */
	if (init_pkt(nf_packet, &pkt, nfq_get_nfmark(tb)) == NOK) {
		g_printerr(
				"%s Packet structure couldn't be initialized, packet dropped\n",
				H(conn->id));
		return to_return;
	}

	/*! Initialize the connection structure (into conn) and get the state of the connection */
	if (init_conn(pkt, &conn) == NOK) {
		g_printerr(
				"%s Connection structure couldn't be initialized, packet dropped\n",
				H(0));
		free_pkt(pkt);
		return to_return;
	}

#ifdef DEBUG
	g_printerr("%s Origin: %s %s, %i bytes\n", H(conn->id),
			lookup_origin(pkt->origin), lookup_state(conn->state), pkt->data);
#endif

	/*! Check that there was no problem getting the current connection structure
	 *  and make sure the STATE is valid */
	if (((conn->state < INIT) && (pkt->origin == EXT))
			|| (conn->state <= INVALID)) {
		///INIT == 1, INVALID == 0 and NOK == -1
		g_printerr("%s Packet not from a valid connection %s\n", H(conn->id),
				inet_ntoa(in));
		if (pkt->packet.ip->protocol == TCP && ICONFIG("reset_ext"))
			reply_reset(&(pkt->packet));

		free_pkt(pkt);
		return to_return;
	}

	if (conn->state == DROP) {
		g_printerr("%s This connection is marked as DROPPED %s\n", H(conn->id),
				inet_ntoa(in));

		if (pkt->packet.ip->protocol == TCP && ICONFIG("reset_ext"))
			reply_reset(&(pkt->packet));

		free_pkt(pkt);
		return to_return;
	}

	// Setup iptables mark on the packet based on what's recorded in the conn_struct
	init_mark(pkt, conn);

	switch (pkt->origin) {
	/*! Packet is from the low interaction honeypot */
	case LIH:
		switch (conn->state) {
		case INIT:
			if (pkt->packet.ip->protocol == TCP && pkt->packet.tcp->syn != 0) {
				conn->hih.lih_syn_seq = ntohl(pkt->packet.tcp->seq);
			}
			store_pkt(conn, pkt);
			//conn->state = CONTROL;
			//switch_state(conn, CONTROL); //Now it's when the connection is created that the state is on CONTROL for LIH
			to_return->statement = 1; //DE_process_packet(pkt);	/*! For now, we don't analyze packets from LIH */
			break;
		case DECISION:
			if (pkt->packet.ip->protocol == TCP && pkt->packet.tcp->syn != 0) {
				conn->hih.lih_syn_seq = ntohl(pkt->packet.tcp->seq);
			}
			store_pkt(conn, pkt);
			to_return->statement = 1; //DE_process_packet(pkt);	/*! For now, we don't analyze packets from LIH */
			break;
		case PROXY:
#ifdef DEBUG
			g_printerr(
					"%s Packet from LIH proxied directly to its destination\n",
					H(conn->id));
#endif
			to_return->statement = 1;
			break;
		case CONTROL:
			if (pkt->packet.ip->protocol == TCP && pkt->packet.tcp->syn != 0) {
				conn->hih.lih_syn_seq = ntohl(pkt->packet.tcp->seq);
			}
			store_pkt(conn, pkt);
			to_return->statement = DE_process_packet(pkt);
			break;
		default:
			g_printerr("%s Packet from LIH at wrong state => reset %s\n",
					H(conn->id), inet_ntoa(in));
			if (pkt->packet.ip->protocol == TCP)
				reply_reset(&(pkt->packet));
			free_pkt(pkt);
			break;
		}
		break;

		/*! Packet is from the high interaction honeypot */
	case HIH:
		switch (conn->state) {
		case REPLAY:
			/*! push the packet to the synchronization list in conn_struct */
			if (pkt->packet.ip->protocol == TCP && pkt->packet.tcp->syn == 1) {
				conn->hih.delta = ~ntohl(pkt->packet.tcp->seq) + 1
						+ conn->hih.lih_syn_seq;
			}
			replay(conn, pkt);
			free_pkt(pkt);
			break;
		case FORWARD:
			forward(pkt);
			free_pkt(pkt);
			break;
			/*! This one should never occur because PROXY are only between EXT and LIH... but we never know! */
		case PROXY:
#ifdef DEBUG
			g_printerr(
					"%s Packet from HIH proxied directly to its destination\n",
					H(conn->id));
#endif
			to_return->statement = 1;
			break;
		case CONTROL:
			to_return->statement = DE_process_packet(pkt);
			free_pkt(pkt);
			break;
		default:
			/*! We are surely in the INIT state, so the HIH is initiating a connection to outside. We reset or control it */
			if (ICONFIG("deny_hih_init") > 0) {
				g_printerr(
						"%s Packet from HIH at wrong state, so we reset %s\n",
						H(conn->id), inet_ntoa(in));
				if (pkt->packet.ip->protocol == TCP) {
					reply_reset(&(pkt->packet));
				}
				to_return->statement = 0;
				//conn->state = DROP;
				switch_state(conn, DROP);
				free_pkt(pkt);
			} else {
				g_printerr(
						"%s Packet from HIH in a new connection, so we control it (%s)\n",
						H(conn->id), inet_ntoa(in));
				switch_state(conn, CONTROL);
				to_return->statement = DE_process_packet(pkt);
				free_pkt(pkt);
			}
			break;
		}
		break;

		/*! Packet is from the external attacker (origin == EXT) */
	default:
		switch (conn->state) {
		case INIT:
			store_pkt(conn, pkt);
			g_string_assign(conn->decision_rule, ";");
			to_return->statement = DE_process_packet(pkt);
			break;
		case DECISION:
			store_pkt(conn, pkt);
			to_return->statement = DE_process_packet(pkt);
			break;
		case FORWARD:
			forward(pkt);
			free_pkt(pkt);
			break;
		case PROXY:
			//#ifdef DEBUG
			g_printerr(
					"%s Packet from EXT proxied directly to its destination (PROXY)\n",
					H(conn->id));
			//#endif
			to_return->statement = 1;
			free_pkt(pkt);
			break;
		case CONTROL:
			//#ifdef DEBUG
			g_printerr(
					"%s Packet from EXT proxied directly to its destination (CONTROL)\n",
					H(conn->id));
			//#endif
			to_return->statement = 1;
			free_pkt(pkt);
			break;
		case DROP:
			to_return->statement = 0;
			free_pkt(pkt);
			break;
		default:
			free_pkt(pkt);
			break;
		}
		break;
	}

	return to_return;
}

/*! q_cb
 *
 \brief Callback function launched by the netfilter queue handler each time a packet is received
 //TODO: push packets here into an internal flow-based queue which the DE_thread can process
 * */
static int q_cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
		struct nfq_data *nfa, void *data) {
	/*! get packet id */
	struct nfqnl_msg_packet_hdr *ph;
	ph = nfq_get_msg_packet_hdr(nfa);
	int id = ntohl(ph->packet_id);

	/* The NAT table only has information about the connection to LIH */
	/* We need to duplicate whatever mark was set on the LIH connection to forwarded connections to HIH */

	/*! launch process function */
	struct verdict *decision = process_packet(nfa);
	int to_return;

	//printf("Final result: %u and set mark to %u\n", decision->statement, decision->mark);

	if (decision->statement == OK) {
		/*! nfq_set_verdict2
		 \brief set a decision NF_ACCEPT or NF_DROP on the packet and put a mark on it
		 *
		 \param[in] qh netfilter queue handle obtained by call to nfq_create_queue
		 \param[in] id id of the packet
		 \param[in] verdict NF_ACCEPT or NF_DROP
		 \param[in] mark netfilter mark value to mark packet with
		 \param[in] data_len (optional) number of bytes of data pointer by buf
		 \param[in] buf pointer to data buffer
		 *
		 \return 0 on success, non-zore on failure */

		/*! ACCEPT the packet if the statement is 1 */
		/* Also copy whatever mark has been on the packet initially, required for multi-uplink setups */

		to_return = nfq_set_verdict2(qh, id, NF_ACCEPT, decision->mark, 0, NULL);
	} else {
		/*! DROP the packet if the statement is 0 (or something else than 1) */
		to_return = nfq_set_verdict2(qh, id, NF_DROP, decision->mark, 0, NULL);
	}

	free(decision);
	return to_return;
}

#ifndef HAVE_LIBEV
/*! netlink loop
 \brief Function to create and maintain the NF_QUEUE loop
 \param[in] queuenum the queue identifier
 \return status
 */
short int netlink_loop(unsigned short int queuenum) {
	struct nfq_handle *h = NULL;
	struct nfq_q_handle *qh = NULL;
	struct nfnl_handle *nh = NULL;
	int fd = -1, rv = -1, watchdog;
	char buf[BUFSIZE];

	running = OK;

	h = nfq_open();
	if (!h)
		errx(1, "%s Error during nfq_open()", __func__);

	if (nfq_unbind_pf(h, AF_INET) < 0)
		errx(1, "%s Error during nfq_unbind_pf()", __func__);

	if (nfq_bind_pf(h, AF_INET) < 0)
		errx(1, "%s Error during nfq_bind_pf()", __func__);

	syslog(LOG_INFO, "NFQUEUE: binding to queue '%hd'\n", queuenum);

	qh = nfq_create_queue(h, queuenum, &q_cb, NULL);
	if (!qh)
		errx(1, "%s Error during nfq_create_queue()", __func__);

	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, PAYLOADSIZE) < 0)
		errx(1, "%s Can't set packet_copy mode", __func__);

	nh = nfq_nfnlh(h);
	fd = nfnl_fd(nh);

	watchdog = 0;
	while (running == OK) {
		memset(buf, 0, BUFSIZE);
		rv = recv(fd, buf, BUFSIZE, 0);
		if (rv < 0) {
			g_printerr("%s Error %d: recv() returned %d '%s'\n", H(0), errno,
					rv, strerror(errno));
			watchdog++;
			if (watchdog > 100) {
				g_printerr(
						"%s Error: too many consecutive failures, giving up\n",
						H(0));
				running = NOK;
			}
		} else {
			nfq_handle_packet(h, buf, rv);
			if (watchdog > 0) {
				watchdog = 0;
			}
		}
	}

	syslog(LOG_INFO,
			"NFQUEUE: unbinding from queue '%hd' (running: %d, rv: %d)\n",
			queuenum, running, rv);
	nfq_destroy_queue(qh);
	nfq_close(h);
	return (0);
}

#else

static void
nfqueue_ev_cb(struct ev_loop *loop, ev_io *w, int revents)
{
	int rv;
	char buf[BUFSIZE];

	rv = recv(w->fd, buf, sizeof(buf), 0);
	if (rv >= 0 && running == OK)
	{
		//nfq_handle_packet((struct nfq_handle *)w->data, buf, rv);
		nfq_handle_packet(h, buf, rv);
	}
}

/*! init_nfqueue
 *
 \brief Function to create the NF_QUEUE loop
 \param[in] queuenum the queue identifier
 \return file descriptor for queue
 */
int
//init_nfqueue(struct nfq_handle *h, struct nfq_q_handle *qh, unsigned short int queuenum)
init_nfqueue(struct nfq_q_handle *qh, unsigned short int queuenum)
{
	struct nfnl_handle *nh;

	running = OK;

	h = nfq_open();
	if (!h)
	errx(1,"%s Error during nfq_open()", __func__);

	if (nfq_unbind_pf(h, AF_INET) < 0)
	errx(1,"%s Error during nfq_unbind_pf()", __func__);

	if (nfq_bind_pf(h, AF_INET) < 0)
	errx(1,"%s Error during nfq_bind_pf()", __func__);

	syslog(LOG_INFO, "NFQUEUE: binding to queue '%hd'\n", queuenum);

	qh = nfq_create_queue(h, queuenum, &q_cb, NULL);
	if (!qh)
	errx(1,"%s Error during nfq_create_queue()", __func__);

	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, PAYLOADSIZE) < 0)
	errx(1,"%s Can't set packet_copy mode", __func__);

	nh = nfq_nfnlh(h);

	return(nfnl_fd(nh));
}

static void
//close_nfqueue(struct nfq_handle *h, struct nfq_q_handle *qh, unsigned short int queuenum)
close_nfqueue(struct nfq_q_handle *qh, unsigned short int queuenum)
{
	syslog(LOG_INFO, "NFQUEUE: unbinding from queue '%hd' (running: %d)\n", queuenum, running);
	nfq_destroy_queue(qh);
	nfq_close(h);
}

static void
timeout_clean_cb (EV_P_ ev_timer *w, int revents)
{
	//g_printerr("%s timeout reach for ev_timer!\n", H(0));
	clean();
}
#endif
//End Test

/*! main
 \brief process arguments, daemonize, init variables, create QUEUE handler and process each packet
 \param[in] argc, number of arguments
 \param[in] argv, table with arguments
 *
 \return 0 if exit with success, anything else if not */
int main(int argc, char *argv[]) {
	int argument;
	char *config_file_name = "";
	FILE *fp;

	unsigned short int queuenum = 0;

#ifdef HAVE_LIBEV
	//struct nfq_handle *h;
	struct nfq_q_handle *qh=NULL;
	int my_nfq_fd;
#endif

#ifdef DEBUG
	g_printerr("\n\n");
#endif
	g_printerr(
			"Honeybrid V%s Copyright (c)\n2007-2009 University of Maryland\n2012 University of Connecticut\n",
			PACKAGE_VERSION);

	/*! parsing arguments */
	if (argc < 2)
		usage(argv);
	while ((argument = getopt(argc, argv, "sc:x:V:q:h:d?")) != -1) {
		switch (argument) {
		case 'c':
			/*! define configuration filename */
			config_file_name = optarg;
			break;
		case 'x':
			/*! send a shutdown request to honeybrid */
			g_printerr("Trying to shutdown honeybrid at pid %s\n", optarg);

			/*! convert argument to int */
			int pid = atoi(optarg);

			/*! check that processus exists */
			if (-1 == kill(pid, 0)) {
				errx(1, "%s: ERROR: Process does not exist", __func__);
			} else {
				g_printerr("%s: Sending signal to halt engine\n", __func__);
				/*! send signal USR1 to PID */
				kill(pid, SIGQUIT);
				exit(0);
			}
			break;
		case 'V':
			printf("Honeybrid Version %s\n", PACKAGE_VERSION);
			exit(0);
			break;
		case 'q':
			queuenum = (unsigned short int) atoi(optarg);
			break;
		case 's':
			g_printerr("Status informations not yet implemented\n");
			exit(-1);
			break;
			/*! \todo
			 add a signal handler to output statistics (figure out the correct file description for output...)
			 Statistics should include:
			 - version
			 - start time and duration
			 - packet processed:
			 * total
			 * TCP
			 * UDP
			 * Other
			 - connection processed:
			 * total
			 * INIT
			 * DECISION
			 * REPLAY
			 * FORWARD
			 * INVALID
			 * PROXY
			 - decision engine, for each module:
			 * rules loaded
			 * signature loaded
			 * connection redirected (MATCH)
			 * connection left alone (DOES NOT MATCH)
			 - errors
			 * NF_QUEUE restarts
			 * expected data
			 - top ports?
			 - top IP addresses?
			 */
			/*case 'd':
			 g_printerr("Daemonizing honeybrid\n");
			 daemonize = 1;
			 break;*/
		case 'h':
		case '?':
		default:
			usage(argv);
			break;
			/* not reached */
		}
	}

	/*! initialize signal handlers */
	init_signal();
	/*! initialize syslog */
	init_syslog(argc, argv);
	/*! initialize data structures */
	init_variables();
	/*! parse the configuration files and store values in memory */
	init_parser(config_file_name);

	/*! Create PID file, we might not be able to remove it */
	pidfile = g_malloc0(
			snprintf(NULL, 0, "%s/honeybrid.pid",
					CONFIG_REQUIRED("exec_directory")) + 1);
	sprintf((char *) pidfile, "%s/honeybrid.pid",
			CONFIG_REQUIRED("exec_directory"));
	unlink(pidfile);
	if ((fp = fopen(pidfile, "w")) == NULL) {
		err(1, "fopen: %s", pidfile);
	}
	fprintf(fp, "%d\n", getpid());
	fclose(fp);

	chmod(pidfile, 0644);
	mainpid = getpid();

	max_packet_buffer = ICONFIG("max_packet_buffer");

	output_t output = ICONFIG_REQUIRED("output");

	/* Start Honeybrid in the background if necessary */
	if (ICONFIG("daemonize") > 0 && output != OUTPUT_STDOUT) {
		g_printerr("Honeybrid starting as background process\n");

		if (daemon(1, 0) < 0) {
			unlink(pidfile);
			err(1, "daemon");
		}
	}

	/* Setting debug file */
	if (output != OUTPUT_STDOUT) {
		setlogmask(LOG_UPTO(LOG_INFO));

		// Only redirect to debug log if we are not compiled _with_ debug
#ifndef DEBUG
		if (ICONFIG("debug") > 0) {
			if ((fdebug = open_debug_log()) != -1) {
				(void) dup2(fdebug, STDIN_FILENO);
				(void) dup2(fdebug, STDOUT_FILENO);
				(void) dup2(fdebug, STDERR_FILENO);
				if (fdebug > 2)
					(void) close(fdebug);
				syslog(LOG_INFO, "done");
			} else {
				syslog(LOG_INFO, "file: %s", strerror(errno));
			}
		}
#endif
	}

	if (output == OUTPUT_MYSQL)
#ifdef HAVE_MYSQL
		init_mysql_log();
#else
		errx(1, "%s: Honeybrid wasn't compiled with MySQL!", __func__);
#endif

	if (output == OUTPUT_LOGFILES)
		open_connection_log();

#ifdef DE_THREAD
	//TODO: Implement this
	//TODO: Multiple DE threads to process faster?
	/*! init the Decision Engine thread */
	/*if( ( thread_de = g_thread_create_full ((void *)DE_submit_packet, NULL, 0, TRUE, TRUE, 0, NULL)) == NULL)
	 errx(1, "%s: Unable to start the decision engine thread", __func__);
	 else
	 g_printerr("%s: Decision engine thread started\n", __func__);*/
#endif

	/*! initiate modules that can have only one instance */
	init_modules();

	/*! create the raw sockets for UDP/IP and TCP/IP */
	if(NOK == init_raw_sockets())
		errx(1, "%s: failed to create the raw sockets", __func__);

#ifdef HAVE_LIBEV
	loop = ev_default_loop(0);
	//Watcher for cleaning conn_tree every 10 seconds:
	ev_timer timeout_clean_watcher;
	ev_timer_init (&timeout_clean_watcher, timeout_clean_cb, 10., 10.);
	ev_timer_start (loop, &timeout_clean_watcher);
	/*! Watcher for processing packets received on NF_QUEUE: */
	my_nfq_fd = init_nfqueue(qh, queuenum);
	ev_io queue_watcher;
	ev_io_init (&queue_watcher, nfqueue_ev_cb, my_nfq_fd, EV_READ);
	ev_io_start (EV_A_ &queue_watcher);
	g_printerr("%s Starting ev_loop\n", H(0));
	//ev_loop (loop, EVLOOP_NONBLOCK);
	ev_loop(EV_A_ 0);
	/*! To be moved inside close_all() */
	close_nfqueue(qh, queuenum);
#else
	/*! create a thread for the management, cleaning stuffs and so on */
	if ((thread_clean = g_thread_new("cleaner", (void *) clean, NULL))
			== NULL) {
		errx(1, "%s: Unable to start the cleaning thread", __func__);
	} else {
		g_printerr("%s: Cleaning thread started\n", __func__);
	}
	/*! Starting the nfqueue loop to start processing packets */
	g_printerr("%s Starting netlink_loop\n", H(0));
	netlink_loop(queuenum);

#endif

	close_all();
	g_printerr("%s: Halted\n", __func__);
	exit(0);

	/* \todo to include in programmer documentation:
	 //What we should use to log messages:
	 //For debugging:
	 g_printerr("%smessage\n", H(30));

	 //For processing information:
	 syslog(LOG_INFO,"message");

	 //For critical warning
	 warn("open");
	 warnx("%s message", __func__);

	 //For fatal error
	 err("fopen");
	 errx(1,"%s message", __func__);

	 */
}
