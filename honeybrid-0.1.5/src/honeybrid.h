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

#include "types.h"

int daemon(int, int);

int yyparse(void);
extern FILE *yyin;

/*! Decision Engine thread enabled */
//#define DE_THREAD
/*! Two strategies: with thread or with libev 
 *  If HAVE_LIBEV is defined, the program loops on the main libev loop:
 *	- packets on queue are processed through libev callback
 *	- connection structures are cleaned by libev timer
 *  If not, then the program loops on nfqueue:
 *	- packets on queue are processed through nfqueue callback
 *	- connection structures are cleaned by a thread
 */
//#define HAVE_LIBEV
