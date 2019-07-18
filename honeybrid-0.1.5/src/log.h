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

#ifndef __LOG_H_
#define __LOG_H_

#include "types.h"
#include "structs.h"

#define L(sdata,ddata,level,id) \
	if (0 != honeylog(sdata,ddata,level,id)) \
	{g_print("******LOG ENGINE ERROR******\n");}
#define H(id) 				log_header(__func__, id)

const char* log_header(const char* function_name, int id);
const char* now(void);

status_t honeylog(char *sdata, char *ddata, log_verbosity_t level, unsigned id);

int open_debug_log(void);

int close_connection_log(void);

void open_connection_log(void);

//void rotate_log(int signal_nb, void *siginfo, void *context);
void rotate_connection_log(int signal_nb);

//void connection_stat(struct conn_struct *conn);
void connection_log();

status_t log_mysql(const struct conn_struct *conn, const GString *proto,
		const GString *status, GString **status_info, gdouble duration);

status_t log_csv(const struct conn_struct *conn, const GString *proto,
		const GString *status, GString **status_info, gdouble duration,
		output_t output);

status_t log_std(const struct conn_struct *conn, const GString *proto,
		const GString *status, GString **status_info, gdouble duration,
		output_t output);

int init_mysql_log();

#endif ///__LOG_H_
