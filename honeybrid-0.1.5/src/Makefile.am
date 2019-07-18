# This file is part of the honeybrid project.
#
# 2007-2009 University of Maryland (http://www.umd.edu)
# (Written by Robin Berthier <robinb@umd.edu>, Thomas Coquelin <coquelin@umd.edu> and Julien Vehent <julien@linuxwall.info> for the University of Maryland)
#
# 2012-2013 University of Connecticut (http://www.uconn.edu)
# (Extended by Tamas K Lengyel <tamas.k.lengyel@gmail.com>
#
# Honeybrid is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.

sbin_PROGRAMS = honeybrid

honeybrid_SOURCES =  honeybrid.c honeybrid.h
honeybrid_SOURCES += types.h globals.h
honeybrid_SOURCES += constants.c constants.h
honeybrid_SOURCES += structs.c structs.h
honeybrid_SOURCES += convenience.c convenience.h
honeybrid_SOURCES += connections.c connections.h
honeybrid_SOURCES += decision_engine.c decision_engine.h
honeybrid_SOURCES += modules.c modules.h
honeybrid_SOURCES += netcode.c netcode.h
honeybrid_SOURCES += log.c log.h
honeybrid_SOURCES += err.c daemon.c
honeybrid_SOURCES += config_rules.y config_syntax.l

honeybrid_SOURCES += mod_control.c
honeybrid_SOURCES += mod_counter.c
honeybrid_SOURCES += mod_hash.c
honeybrid_SOURCES += mod_random.c
honeybrid_SOURCES += mod_source.c
honeybrid_SOURCES += mod_source_time.c
honeybrid_SOURCES += mod_yesno.c
honeybrid_SOURCES += mod_dionaea.c mod_dionaea.h
honeybrid_SOURCES += mod_backpick_random.c
honeybrid_SOURCES += mod_vmi.c

# Compiler flags:
AM_CFLAGS = $(CRYPTO_CFLAGS)
AM_CFLAGS += $(GLIB_CFLAGS)
AM_CFLAGS += $(XML2_CFLAGS)
AM_CFLAGS += $(MYSQL_CFLAGS)
AM_CFLAGS += $(HARDEN_CFLAGS)

# Note that -pg is incompatable with $(HARDEN_LDFLAGS)
#AM_CFLAGS += -Wall -Wextra -g -ggdb3 -pg -DDEBUG

AM_LDFLAGS =  $(CRYPTO_LIBS)
AM_LDFLAGS += $(GLIB_LIBS)
AM_LDFLAGS += $(XML2_LIBS)
AM_LDFLAGS += $(MYSQL_LIBS)
AM_LDFLAGS += $(CORE_LIBS)
AM_LDFLAGS += $(HARDEN_LDFLAGS)

AM_YFLAGS= -tvy -d -v
AM_LFLAGS= -o$(LEX_OUTPUT_ROOT).c
