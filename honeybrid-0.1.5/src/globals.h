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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "types.h"

/*! \brief the pid of the main program
 */
long int mainpid;

const char *pidfile;

/*!
 \def threading
 * Init value: OK
 * Set to NOK when honeybrid stops, used to terminate threads
 */
status_t threading;

GMutex threading_cond_lock;
GCond threading_cond;

/*! \brief connection id */
uint64_t c_id;

/*! \brief max number of packets to save for replay in an EXT<->LIH connection (negative value = save all) */
int max_packet_buffer;

/*! \brief global array of pointers to hold target structures */
GPtrArray *targets;

/*! \brief global hash table that contain the values of the configuration file  */
GHashTable *config;

/*! \brief global hash table to hold module paramaters */
GHashTable *module;

/*! \brief global hash table to hold module paramaters */
GHashTable *uplink;

/*! \brief global hash table that contain the dynamic correspondance between HIH services et LIH services  */
GHashTable *high_redirection_table;

/*! \brief security writing lock for the dynamic high interaction redirection table
 */
GRWLock hihredirlock;

/*! \brief Balanced Binary Tree that keep meta informations about active connections
 *
 \param key, each entry is represented by the tuple of the connection (sourceIP+sourcePort+destIP+destPort)
 \param value, the associated value of an entry is a conn_struct structure  */
GTree *conn_tree;

/*! \brief security writing lock for the Binary Tree
 */
GRWLock conntreelock;

/*! \def list of module to save
 */
GHashTable *module_to_save;

/*! \brief pointer table for btree cleaning */
GPtrArray *entrytoclean;

/*!
 \def running
 *
 * Init value: OK
 * Set to NOK when honeybrid stops
 * It is used to stop processing new data wht NF_QUEUE when honeybrid stops */
status_t running;

/*!
 \def thread_clean
 \def thread_log */
GThread *thread_clean;
GThread *thread_de;
GThread *mod_backup;

/*!
 \def log level
 */

log_verbosity_t LOG_LEVEL;

/*!
 \Def file descriptor to log debug output
 */
int fdebug;

/* -------------------------------------------------- */

#endif //__GLOBALS_H__
