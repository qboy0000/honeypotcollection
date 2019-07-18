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

#include <errno.h>

#include "modules.h"

#define ATTACK_TIMEOUT 600

GThread vmi_status_updater;
GTree *vmi_vms;
GTree *bannedIPs;
int vmi_sock;

struct sockaddr_in vmi_addr; /* VMI server address */
unsigned short vmi_port; /* VMI server port */

struct vmi_vm {
	gchar *attacker;
	char *name;
	uint32_t vmID;
	uint32_t logID;
	int socket;
	int paused;
	gint start;
	gint last_seen;
	gint event;

	GRWLock lock;

	// To manipulate connections associated
	// we need to keys
	GSList *conn_keys;
};

struct vm_search {
	gchar *srcIP;
	struct vmi_vm *vm;
};

struct attacker_search {
	gchar *srcIP;
	gchar *outIP;
	gchar *attacker;
	uint32_t mark;
	gint banned;

	struct vmi_vm *vm;
	gint found;
	gint event;
};

const char* vmi_log(gpointer data) {
	static char vmi_log_buff[12];
	snprintf(vmi_log_buff, 12, "'%u'", GPOINTER_TO_UINT(data));
	return vmi_log_buff;
}

void free_vmi_vm(gpointer data) {
	struct vmi_vm *vm = (struct vmi_vm *) data;
	if (vm->attacker != NULL)
		free(vm->attacker);
	if (vm->name != NULL)
		free(vm->name);

	free(vm);
}

// Each backend
gboolean build_vmi_vms2(gpointer key, gpointer value, gpointer data) {

	//int *vmi_sock=(int *)data;
	struct backend *back_handler = (struct backend *) value;
	char *vm_name = back_handler->iface->tag;
	char vmi_buffer[100];
	bzero(vmi_buffer, 100);
	sprintf(vmi_buffer, "status,%s\n", vm_name);

	//printf("Sending on socket %i: %s", vmi_sock, vmi_buffer);

	if (write(vmi_sock, vmi_buffer, strlen(vmi_buffer)) < 0) {
		g_printerr("%s Couldn't send message to VMI server\n", H(22));
	} else {
		bzero(vmi_buffer, 100);

		if (read(vmi_sock, vmi_buffer, 100) < 0) {
			printf("%s Didn't get any message back!\n", H(22));
		} else {

			char *nl = strrchr(vmi_buffer, '\r');
			if (nl)
				*nl = '\0';
			nl = strrchr(vmi_buffer, '\n');
			if (nl)
				*nl = '\0';

			if (!strcmp(vmi_buffer, "active")) {
				g_printerr("%s VM %s is active, pausing!\n", H(22), vm_name);

				bzero(vmi_buffer, 100);
				sprintf(vmi_buffer, "pause,%s\n", vm_name);
				if(write(vmi_sock, vmi_buffer, strlen(vmi_buffer))==0)
					return FALSE;
				bzero(vmi_buffer, 100);
				if(read(vmi_sock, vmi_buffer, 100)==0)
					return FALSE;
				if (strcmp(vmi_buffer, "paused\n\r")) {
					g_printerr(
							"%s VM was not paused on our request, skipping\n",
							H(22));
					return FALSE;
				} else
					g_printerr("%s VM was paused, enabling\n", H(22));

				struct vmi_vm *vm = g_malloc0(sizeof(struct vmi_vm));

				vm->vmID = *(uint32_t*) key;
				vm->name = strdup(vm_name);
				vm->attacker = NULL;
				vm->paused = 1;
				vm->event = 0;
				g_rw_lock_init(&(vm->lock));

				g_tree_insert(vmi_vms, (gpointer) vm->name, (gpointer) vm);

			} else if (!strcmp(vmi_buffer, "paused")) {

				g_printerr("%s VM %s is paused, enabling!\n", H(22), vm_name);
				struct vmi_vm *vm = g_malloc0(sizeof(struct vmi_vm));

				vm->vmID = *(uint32_t*) key;
				vm->name = strdup(vm_name);
				vm->attacker = NULL;
				vm->paused = 1;
				vm->logID = 0;
				vm->event = 0;
				g_rw_lock_init(&(vm->lock));

				g_tree_insert(vmi_vms, (gpointer) vm->name, (gpointer) vm);
			} else
				g_printerr("%s VM %s is inactive!\n", H(22), (char *) value);
		}
	}

	return FALSE;
}

// Loop each target
void build_vmi_vms(gpointer data, gpointer user_data) {

	struct target *t = (struct target *) data;
	g_tree_foreach(t->back_handlers, (GTraverseFunc) build_vmi_vms2, user_data);

}

gboolean find_free_vm(gpointer key, gpointer value, gpointer data) {

	//uint32_t *vmID=(uint32_t *)key;
	struct vmi_vm *vm = (struct vmi_vm *) value;
	struct vm_search *search = (struct vm_search *) data;
	int found = 0;

	//printf("Searching for free VM: %s\n", vm->name);

	g_rw_lock_writer_lock(&(vm->lock));
	if (vm->paused) {

		char buf[100];
		bzero(buf, 100);
		sprintf(buf, "activate,%s\n", vm->name);

		int n = write(vmi_sock, buf, strlen(buf));
		if (n < 0)
			errx(1, "%s ERROR writing to socket\n", __func__);

		bzero(buf, 100);
		n = read(vmi_sock, buf, 100);
		if (n <= 0)
			errx(1, "%s Error receiving from Honeymon!\n", __func__);

		char *p;
		char delim[] = ",";
		strtok_r(buf, delim, &p);

		vm->logID = atoi(strtok_r(NULL, delim, &p));
		vm->paused = 0;
		vm->attacker = g_strdup(search->srcIP);
		GTimeVal t;
		g_get_current_time(&t);
		vm->start = (t.tv_sec);
		vm->event = 0;

		search->vm = vm;
		found = 1;

		g_printerr("%s Found free VM and activated it: %u!\n", H(22), vm->vmID);
	}
	g_rw_lock_writer_unlock(&(vm->lock));

	if (found)
		return TRUE;
	else
		return FALSE;
}

void get_random_vm(struct vm_search *search) {

	int n = write(vmi_sock, "random\n", strlen("random\n"));
	if (n < 0)
		errx(1, "%s ERROR writing to socket\n", __func__);

	char buf[100];
	bzero(buf, 100);
	n = read(vmi_sock, buf, 100);
	if (n <= 0)
		errx(1, "%s Error receiving from Honeymon!\n", __func__);

	char *nl = strrchr(buf, '\r');
	if (nl)
		*nl = '\0';
	nl = strrchr(buf, '\n');
	if (nl)
		*nl = '\0';

	if (strcmp("-", buf)) {
		struct vmi_vm *vm = (struct vmi_vm *) g_tree_lookup(vmi_vms, buf);
		if (vm != NULL) {
			bzero(buf, 100);
			sprintf(buf, "activate,%s\n", vm->name);

			n = write(vmi_sock, buf, strlen(buf));
			if (n < 0)
				errx(1, "%s ERROR writing to socket\n", __func__);

			bzero(buf, 100);
			n = read(vmi_sock, buf, 100);
			if (n <= 0)
				errx(1, "%s Error receiving from Honeymon!\n", __func__);

			char *p;
			char delim[] = ",";
			strtok_r(buf, delim, &p);

			g_rw_lock_writer_lock(&(vm->lock));
			vm->logID = atoi(strtok_r(NULL, delim, &p));
			vm->paused = 0;
			vm->attacker = g_strdup(search->srcIP);
			GTimeVal t;
			g_get_current_time(&t);
			vm->start = (t.tv_sec);
			vm->event = 0;
			g_rw_lock_writer_unlock(&(vm->lock));

			search->vm = vm;
		} else {
			g_tree_insert(bannedIPs, (gpointer) strdup(search->srcIP), NULL);
		}
	} else {
		g_tree_insert(bannedIPs, (gpointer) strdup(search->srcIP), NULL);
	}
}

gboolean find_used_vm(gpointer key, gpointer value, gpointer data) {
	struct vmi_vm *vm = (struct vmi_vm *) value;
	struct vm_search *search = (struct vm_search *) data;

	//printf("Searching for used VM: %s (%u)\n", vm->name, vm->vmID);
	g_rw_lock_reader_lock(&(vm->lock));
	if (!vm->paused) {
		if (!strcmp(search->srcIP, vm->attacker)) {
			printf("%s This attacker is already using a VM: %u\n", H(65),
					vm->vmID);
			search->vm = vm;
			g_rw_lock_reader_unlock(&(vm->lock));
			return TRUE;
		}
	}
	g_rw_lock_reader_unlock(&(vm->lock));

	return FALSE;
}

gboolean find_if_banned(gpointer key, gpointer value, gpointer data) {
	struct attacker_search *attacker = (struct attacker_search *) data;
	if (!strcmp((char *) key, attacker->srcIP)) {
		attacker->banned = 1;
		return TRUE;
	}

	return FALSE;
}

void mod_vmi_front(struct mod_args *args) {
	g_printerr("%s VMI Front Module called\n", H(args->pkt->conn->id));

	gchar **values = g_strsplit(args->pkt->key_src, ":", 0);
	/*struct attacker_search attacker;
	 attacker.srcIP = values[0];
	 attacker.banned = 0;*/

	//printf("Check if attacker is banned..\n");
	//g_tree_foreach(bannedIPs, (GTraverseFunc)find_if_banned,(gpointer)&attacker);
	//if(attacker.banned==0) {
	struct vm_search vm;
	vm.srcIP = values[0];
	vm.vm = NULL;

	g_tree_foreach(vmi_vms, (GTraverseFunc) find_used_vm, &vm);
	if (vm.vm != NULL) {
		g_rw_lock_writer_lock(&(vm.vm->lock));
		if (!vm.vm->paused) {
			GTimeVal t;
			g_get_current_time(&t);
			vm.vm->last_seen = (t.tv_sec);
		}
		g_rw_lock_writer_unlock(&(vm.vm->lock));
	}

	args->node->result = 1;
	//} else {
	//	args->node->result = 0;
	//}
	g_strfreev(values);

}

void mod_vmi_pick(struct mod_args *args) {

	g_printerr("%s VMI Backpick Module called\n", H(args->pkt->conn->id));

	struct vm_search search;
	gchar **values = g_strsplit(args->pkt->key_src, ":", 0);
	search.srcIP = values[0];
	search.vm = NULL;

	struct attacker_search attacker;
	attacker.srcIP = values[0];
	attacker.banned = 0;

	//printf("Check if attacker is banned..\n");
	g_tree_foreach(bannedIPs, (GTraverseFunc) find_if_banned,
			(gpointer) &attacker);

	if (attacker.banned == 0) {

		//printf("Check if he already uses a clone\n");

		g_tree_foreach(vmi_vms, (GTraverseFunc) find_used_vm,
				(gpointer) &search);

		if (search.vm == NULL) {

			//printf("Check if we have any clones available\n");

			//g_tree_foreach(vmi_vms, (GTraverseFunc)find_free_vm, (gpointer)&search);
			get_random_vm(&search);
		}
	} else {
		printf("%s Attacker %s is banned!\n", H(64), attacker.srcIP);
	}

	if (search.vm != NULL) {
		//printf("%s Picking %s (%u).\n", H(args->pkt->conn->id), search.vm->name, search.vm->vmID);
		args->backend_use = search.vm->vmID;
		args->node->result = 1;

		struct custom_conn_data *log = g_malloc0(
				sizeof(struct custom_conn_data));
		log->data = GUINT_TO_POINTER(search.vm->logID);
		log->data_print = vmi_log;

		args->pkt->conn->custom_data = g_slist_append(
				args->pkt->conn->custom_data, log);

		// save the connection keys
		g_rw_lock_writer_lock(&(search.vm->lock));
		search.vm->conn_keys = g_slist_append(search.vm->conn_keys,
				strdup(args->pkt->conn->key));
		g_rw_lock_writer_unlock(&(search.vm->lock));

	} else {
		g_printerr("%s No available backend found, rejecting!\n",
				H(args->pkt->conn->id));
		args->node->result = 0;
	}

	g_strfreev(values);
}

//void mod_vmi_back(struct mod_args *args) {
	/*	g_printerr("%s VMI Back Module called\n", H(args->pkt->conn->id));

	 GTimeVal t;
	 g_get_current_time(&t);
	 gint now = (t.tv_sec);

	 // Check if IP is banned (exceeded allowed time-frame)
	 gchar **key_src;
	 key_src = g_strsplit( args->pkt->key_src, ":", 0);
	 gint *start=(gint *)g_tree_lookup(bannedIPs, key_src[0]);
	 g_strfreev(key_src);

	 if(start!=NULL && now-(*start)>ATTACK_TIMEOUT) {
	 args->node->result = 0;
	 return;
	 }

	 int n=write(vmi_sock, "free\n", strlen("free\n"));

	 if(n<=0) { args->node->result = 0; return; }

	 char buf[100];
	 bzero(buf,100);
	 n=read(vmi_sock,buf,100);

	 if(n<=0) { args->node->result = 0; return; }

	 char *nl = strrchr(buf, '\r');
	 if (nl) *nl = '\0';
	 nl = strrchr(buf, '\n');
	 if (nl) *nl = '\0';

	 int free=atoi(buf);

	 if(free>0) {
	 args->node->result = 1;

	 key_src = g_strsplit( args->pkt->key_src, ":", 0);
	 char *attacker=strdup(key_src[0]);
	 g_strfreev(key_src);
	 gint *startTime=malloc(sizeof(gint));
	 *startTime=now;

	 g_tree_insert(bannedIPs, (gpointer)attacker, (gpointer)startTime);
	 }
	 else		args->node->result = 0;
	 */
//}

gboolean control_check_attacker(gpointer key, gpointer value, gpointer data) {
	struct vmi_vm *vm = (struct vmi_vm *) value;
	struct attacker_search *search = (struct attacker_search *) data;

	GTimeVal t;
	g_get_current_time(&t);
	gint now = (t.tv_sec);
	int found = 0;

	g_rw_lock_writer_lock(&(vm->lock));
	if (!vm->paused
			&& vm->vmID == search->mark&& now-(vm->start)<=ATTACK_TIMEOUT) {

printf			("%s Attack session found on clone %s!\n", H(67), vm->name);

			search->found=1;
			search->attacker=strdup(vm->attacker);
			search->vm=vm;

			if(vm->event)
			search->event=1;
			else
			// Is the clone going somewhere else than the attacker?
			if(strcmp(search->outIP, vm->attacker)) {
				search->event=1;

				vm->event=1;
			}

			found=1;
		}
	g_rw_lock_writer_unlock(&(vm->lock));

	if (found)
		return TRUE;
	else
		return FALSE;
}

void mod_vmi_control(struct mod_args *args) {
	// Only control packets coming from the clones
	if (args->pkt->mark == 0) {
		args->node->result = 1;
		return;
	}

	g_printerr("%s VMI Control Module called\n", H(args->pkt->conn->id));

	/*! get the IP address from the packet */
	gchar **key_dst;
	//key_src = g_strsplit( args->pkt->key_src, ":", 0);
	key_dst = g_strsplit(args->pkt->key_dst, ":", 2);

	//g_printerr("Control module is looking for an attack session from %s\n", key_dst[0]);

	struct attacker_search search;
	search.found = 0;
	search.event = 0;
	search.outIP = key_dst[0];
	search.attacker = NULL;
	search.mark = args->pkt->mark;
	search.vm = NULL;

	g_tree_foreach(vmi_vms, (GTraverseFunc) control_check_attacker,
			(gpointer) &search);

	if (search.found && search.vm != NULL) {

		struct custom_conn_data *log = g_malloc0(
				sizeof(struct custom_conn_data));
		log->data = GUINT_TO_POINTER(search.vm->logID);
		log->data_print = vmi_log;

		args->pkt->conn->custom_data = g_slist_append(
				args->pkt->conn->custom_data, log);

		if (search.event) {
			g_printerr("%s Cought network event, sending signal!\n",
					H(args->pkt->conn->id));
			args->node->result = 0;

			char *buf = g_malloc0(
					snprintf(NULL, 0, "%s,%s,%s\n", search.vm->name,
							search.attacker, search.outIP) + 1);
			sprintf(buf, "%s,%s,%s\n", search.vm->name, search.attacker,
					search.outIP);
			if(write(vmi_sock, buf, strlen(buf))<0)
				g_printerr("%s Failed to write to socket!\n", H(args->pkt->conn->id));
			free(buf);
		} else
			args->node->result = 1;

		free(search.attacker);
	} else {
		args->node->result = 0;
	}

	g_strfreev(key_dst);
	return;
}

void mod_vmi(struct mod_args *args) {

	gchar *mode;
	/*! get the backup file for this module */
	if (NULL
			== (mode = (gchar *) g_hash_table_lookup(args->node->arg, "mode"))) {
		/*! We can't decide */
		args->node->result = -1;
		g_printerr("%s mandatory argument 'mode' undefined (back/control)!\n",
				H(args->pkt->conn->id));
		return;
	}
	//else
	//printf("VMI Mode %s\n", mode);

	if (!strcmp(mode, "front"))
		mod_vmi_front(args);
	else if (!strcmp(mode, "pick"))
		mod_vmi_pick(args);
	//else if (!strcmp(mode, "back"))
	//	mod_vmi_back(args);
	else if (!strcmp(mode, "control"))
		mod_vmi_control(args);
	else
		args->node->result = -1;
}

//////////////////////

void vm_status_updater_thread() {

	printf("%s Honeymon VM status updater thread started\n", H(66));

	int vmi_sock_updater = socket(AF_INET, SOCK_STREAM, 0);
	if (vmi_sock_updater < 0)
		errx(1, "%s: ERROR opening socket", __func__);

	if (connect(vmi_sock_updater, (struct sockaddr *) &vmi_addr,
			sizeof(vmi_addr)) < 0)
		errx(1, "%s: ERROR connecting", __func__);

	char delim[] = ",";
	char *signal = malloc(snprintf(NULL, 0, "listening\n") + 1);
	sprintf(signal, "listening\n");
	while (1) {
		int n = write(vmi_sock_updater, signal, strlen(signal));
		if (n < 0) {
			printf("Updater thread failed to send signal: %s!\n",
					strerror(errno));
			break;
		}

		char buf[100];
		bzero(buf, 100);
		n = read(vmi_sock_updater, buf, 100);

		if (n > 0) {

			char *nl = strrchr(buf, '\r');
			if (nl)
				*nl = '\0';
			nl = strrchr(buf, '\n');
			if (nl)
				*nl = '\0';
			char *p;

			char *first = strtok_r(buf, delim, &p);
			char *second = strtok_r(NULL, delim, &p);
			if (!strcmp(first, "reverted") && second != NULL) {

				struct vmi_vm *vm = g_tree_lookup(vmi_vms, (gpointer) second);

				if (vm != NULL) {
					g_rw_lock_writer_lock(&(vm->lock));

					if (vm->attacker != NULL) {

						g_printerr(
								"%s Setting connections to DROP and banning %s\n",
								H(62), vm->attacker);

						GSList *r = vm->conn_keys;
						struct conn_struct *conn;
						if (r != NULL) {
							g_rw_lock_writer_lock(&conntreelock);
							while (r != NULL) {
								conn = (struct conn_struct *) g_tree_lookup(
										conn_tree, r->data);
								if (conn != NULL)
									conn->state = DROP;
								free((char *) r->data);
								r = r->next;
							}
							g_rw_lock_writer_unlock(&conntreelock);
						}
						g_slist_free(vm->conn_keys);

						g_tree_insert(bannedIPs,
								(gpointer) strdup(vm->attacker),
								(gpointer) NULL);
						free(vm->attacker);
					}

					vm->attacker = NULL;
					vm->conn_keys = NULL;

					vm->paused = 1;
					vm->event = 0;

					g_rw_lock_writer_unlock(&(vm->lock));

				}
			}
		}
	}

	free(signal);
}

int init_mod_vmi() {

	gchar *vmi_server_ip, *vmi_server_port;

	if (NULL
			== (vmi_server_ip = (gchar *) g_hash_table_lookup(config,
					"vmi_server_ip"))) {
		// Not defined so skipping init
		return 0;
	}

	if (NULL
			== (vmi_server_port = (gchar *) g_hash_table_lookup(config,
					"vmi_server_port"))) {
		errx(1, "%s: VMI Server port not defined!!\n", __func__);
	}

	g_printerr("%s Init mod vmi\n", H(22));

	/* socket: create the socket */
	vmi_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (vmi_sock < 0)
		errx(1, "%s: ERROR opening socket", __func__);

	/* build the server's Internet address */
	bzero((char *) &vmi_addr, sizeof(vmi_addr));
	vmi_addr.sin_family = AF_INET;
	vmi_addr.sin_addr.s_addr = inet_addr(vmi_server_ip);
	vmi_addr.sin_port = htons(atoi(vmi_server_port));

	/* connect: create a connection with the server */
	if (connect(vmi_sock, (struct sockaddr *) &vmi_addr, sizeof(vmi_addr)) < 0)
		errx(1, "%s: ERROR connecting", __func__);

	int n = write(vmi_sock, "hello\n", strlen("hello\n"));
	if (n < 0)
		errx(1, "%s ERROR writing to socket\n", __func__);

	char buf[100];
	bzero(buf, 100);
	n = read(vmi_sock, buf, 100);
	if (n < 0 || strcmp(buf, "hi\n\r"))
		errx(1, "%s Error receiving from Honeymon!\n", __func__);
	else
		printf("%s VMI-Honeymon is active, query VM states..\n", H(22));

	vmi_vms = g_tree_new_full((GCompareDataFunc) strcmp, NULL,
			NULL, (GDestroyNotify) free_vmi_vm);
	g_ptr_array_foreach(targets, (GFunc) build_vmi_vms, (gpointer) &vmi_sock);

	bannedIPs = g_tree_new_full((GCompareDataFunc) strcmp, NULL,
			(GDestroyNotify) g_free, NULL);

	g_thread_new("vm_status_updater", (void *) vm_status_updater_thread, NULL);

	return 0;
}
