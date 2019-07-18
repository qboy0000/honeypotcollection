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

ACLOCAL_AMFLAGS = --install -I m4
EXTRA_DIST =  honeybrid.conf honeybrid.sh doc scripts tools
SUBDIRS = src @doc@ @xmpp_listener@

install-data-am:
	@echo
	@echo "========================================="
	@echo "     Thanks for using Honeybrid!"
	@echo
	@echo "To complete the installation on Linux,"
	@echo "you can type the following commands:"
	@echo " sudo mkdir /etc/honeybrid"
	@echo " sudo cp ./honeybrid.conf /etc/honeybrid/"
	@echo " sudo mkdir /var/log/honeybrid"
	@echo " sudo cp ./honeybrid.sh /etc/init.d/"
	@echo
	@echo "Do not forget to edit honeybrid.conf"
	@echo "before running:"
	@echo " sudo /etc/init.d/honeybrid.sh start"
	@echo 
	@echo "Please submit bugs and suggestions on"
	@echo "honeybrid.sf.net or to tamas.k.lengyel@gmail.com"	
	@echo "========================================="
	@echo

