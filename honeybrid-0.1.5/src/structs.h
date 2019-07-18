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

#ifndef __STRUCTS_H_
#define __STRUCTS_H_

#include "types.h"

/*!
 \def backend
 \brief structure to hold backend information (id, decision rule and interface information)
 */
struct backend {
	uint32_t id;
	struct node *rule;
	struct interface *iface;
};

void free_backend(struct backend *);

/*!
 \def target
 \brief structure to hold target information: PCAP filter and rules to accept/forward/redirect/control packets
 */
struct target {
	struct bpf_program *filter; /* PCAP compiled filter to select packets for this target */
	struct addr *front_handler; /* Honeypot IP address(es) handling the first response (front end) */
	struct node *front_rule; /* Rules of decision modules to accept packet to be handled by the frontend */
	GTree *back_handlers; /* Honeypot backends handling the second response with key: hihID, value: struct backend */
	GHashTable *unique_backend_ips; /* Unique backend IPs of back_handlers */
	struct node *back_picker; /* Rule(s) to pick which backend to use (such as VM name, etc.) */
	struct node *control_rule; /* Rules of decision modules to limit outbound packets from honeypots */
	uint32_t backends; /* Number of backends specified */
};

void free_target(gpointer data);
void free_target_gfunc(struct target *, gpointer user_data);

/*!
 \def ethernet_hdr
 \brief memory structure to hold ethernet header (14 bytes)
 */
struct ethernet_hdr {
	u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
	u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
	u_short ether_type; /* IP? ARP? RARP? etc */
};

/*!
 \def packet
 *
 \brief The IP packet structure
 *
 \param ip, ip header
 \param payload[BUFSIZE], payload buffer
 *
 */
struct packet {
	struct iphdr *ip;
	union {
		const struct tcphdr *tcp;
		const struct udphdr *udp;
	};
	const char *payload;
	char *FRAME;
};

/*!
 \def tcp_packet
 *
 \brief The TCP/IP packet structure
 *
 \param ip, ip header
 \param tcp, tcp header
 \param payload[BUFSIZE], payload buffer
 *
 */
struct tcp_packet {
	struct iphdr ip;
	struct tcphdr tcp;
	char *payload;
};

struct pseudotcphdr {
	uint32_t saddr;
	uint32_t daddr;
	uint8_t res1;
	uint8_t proto;
	uint16_t tcp_len;
};

struct tcp_chk_packet {
	struct pseudotcphdr pseudohdr;
	struct tcphdr tcp;
	char payload[BUFSIZE];
};

/*!
 \def udp_packet
 *
 \brief The UDP/IP packet structure
 *
 \param ip, ip header
 \param udp, udp header
 \param payload[BUFSIZE], payload buffer
 *
 */
struct udp_packet {
	struct iphdr ip;
	struct udphdr udp;
	char *payload;
};

/*! \brief Structure to hold network interface information
 */
struct interface {
	char *name; // like "eth0"
	struct addr *ip;
	char *ip_str;
	char *tag; // like "main"
	int tcp_socket;
	int udp_socket;
	int mark;
};

void free_interface(struct interface *iface);

/*! hih_struct
 \brief hih info

 \param addr, IP address
 \param port, port
 */
struct hih_struct {
	int hihID;
	int addr;
	struct interface *iface;
	short port;
	unsigned lih_syn_seq;
	unsigned delta;
	int lih_addr;
	char *redirect_key;
};

/*! expected_data_struct
 \brief expected_data_struct info

 \param ip_proto, expected IP following protocol
 \param tcp_seq, expected TCP sequence number
 \param tcp_seq_ack, expected TCP ack number
 \param payload, expected payload
 */
struct expected_data_struct {
	unsigned short ip_proto;
	unsigned tcp_seq;
	unsigned tcp_ack_seq;
	const char* payload;
};

/*! custom_conn_data
 \brief Extra information to be attached to a conn_struct by a module

 \param data, pointer to the data
 \param data_free, function pointer to free the data when the conn_struct is destroyed
 \param data_print, function pointer to convert the data to a string
 */
struct custom_conn_data {
	gpointer data; // the actual data
	gpointer (*data_free)(gpointer data); // define function to free data (if any)
	const char* (*data_print)(gpointer data); // define function to print data in log (if any)
};

/*! conn_struct
 \brief The meta informations of a connection stored in the main Binary Tree

 \param key, the tuple (also the b-tree key)
 \param key_ext, the IP and Port of the external attacker
 \param key_lih, the IP and Port of the Low Interaction Honeypot
 \param key_hih, the IP and Port of the High Interaction Honeypot
 \param protocol, the l4 protocol number (6 for TCP, 17 for UDP, ...)
 \param access_time, the last access time
 \param status, the status of the connection: (1) for INIT, (2) for DECISION, (3) for REPLAY and (4) for FORWARD. (0) can mean INVALID
 \param count_data_pkt_from_lih, nb of packet replied from the lih to the intruder
 \param count_data_pkt_from_intruder, nb of packet sent from the intruder to the LIH
 \param BUFFER, pointer to the beginning of the list of the recorded packets (stored through pkt_struct)
 \param lock, set to 1 when a packet is currently processed for this connection
 \param hih, hih info
 */
struct conn_struct {
	char *key;
	char *key_ext;
	char *key_lih;
	int protocol;
	GString *start_timestamp;
	gdouble start_microtime;
	gint access_time;
	int state;
	unsigned id;
	int replay_id;
	int count_data_pkt_from_lih;
	int count_data_pkt_from_intruder;
	GSList *BUFFER;
	struct expected_data_struct expected_data;
	GRWLock lock;
	struct hih_struct hih;
	origin_t initiator; // who initiated the conn? EXT/LIH/HIH

	struct target *target;

	/* statistics */
	gdouble stat_time[8]; // = {0,0,0,0,0,0,0};
	int stat_packet[8]; // = {0,0,0,0,0,0,0};
	int stat_byte[8]; // = {0,0,0,0,0,0,0};
	int total_packet;
	int total_byte;
	int decision_packet_id;
	GString *decision_rule;
	uint8_t replay_problem;
	int invalid_problem; //unused

	u_int32_t uplink_mark; // adding support for multiple uplinks
	u_int32_t downlink_mark; // adding support for multiple backends on separate bridges

	GSList *custom_data; // allow custom data to be assigned to the connection by modules
						 // the list elements have to point to struct custom_conn_data

#ifdef HAVE_XMPP
uint8_t dionaeaDownload;
unsigned int dionaeaDownloadTime;
#endif
}__attribute__ ((packed));

/*! pkt_struct
 \brief The meta information of a packet stored in the conn_struct connection structure

 \param packet, pointer to the packet
 \param origin, to define from where the packet is coming (EXT, LIH or HIH)
 \param data, to provide the number of bytes in the packet
 \param DE, (0) if the packet was received before the decision to redirect, (1) otherwise
 */
struct pkt_struct {
	struct packet packet;
	origin_t origin;
	int data;
	int size;
	int DE;
	struct conn_struct * conn;
	char *key_src;
	char *key_dst;
	char *key;
	int position;

	u_int32_t mark; // adding support for multiple uplinks
}__attribute__ ((packed));
;

/*! \brief Structure to pass arguments to the Decision Engine
 \param conn, pointer to the refered conn_struct
 \param packetposition, position of the packet to process in the Singly Linked List
 */
struct DE_submit_args {
	struct conn_struct *conn;
	int packetposition;
};

/*! \brief Structure to receive verdict from process_packet in nfq_cb
 \param statement, 0 for reject, 1 for accept
 \param mark, the netfilter mark to set on the packet (if any)
 */

struct verdict {
	status_t statement;
	u_int32_t mark;
};

/*!
 \def mod_args
 *
 \brief arguments sent to a module while processing the tree
 */
struct mod_args {
	struct node *node;
	struct pkt_struct *pkt;
	uint32_t backend_test, backend_use;
};

struct mod_def {
	const char *name;
	const module_function function;
};

/*!
 \def node
 *
 \brief node of an execution tree, composed of a module and a argument, called by processing the tree
 */
struct node {
	module_function module;
	GHashTable *arg;
	GString *module_name;
	GString *function;
	struct node *true;
	struct node *false;
	int result;
	int info_result;
};

/*!
 \def decision_holder
 *
 \brief structure to hold decision input/output of the DE engine
 */
struct decision_holder {
	struct pkt_struct *pkt;
	struct node *node;
	struct mod_args args;
	decision_t result;

	uint32_t backend_test, backend_use;
};

struct log_event {
	char *sdata;
	char *ddata;
	int level;
	unsigned id;
	char *curtime;
};

#endif /* __STRUCTS_H_ */
