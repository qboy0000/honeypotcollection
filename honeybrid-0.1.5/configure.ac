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

AC_PREREQ(2.61)
AC_INIT([honeybrid],
        [0.1.5],
        [Tamas K Lengyel, tamas.k.lengyel@gmail.com],
        [honeybrid])
AM_INIT_AUTOMAKE([1.10 foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AM_PROG_LEX

###### AUTOHARDEN START
# We want to check for compiler flag support, but there is no way to make
# clang's "argument unused" warning fatal.  So we invoke the compiler through a
# wrapper script that greps for this message.
saved_CC="$CC"
saved_CXX="$CXX"
saved_LD="$LD"
flag_wrap="$srcdir/scripts/wrap-compiler-for-flag-check"
CC="$flag_wrap $CC"
CXX="$flag_wrap $CXX"
LD="$flag_wrap $LD"

# We use the same hardening flags for C and C++.  We must check that each flag
# is supported by both compilers.
AC_DEFUN([check_cc_cxx_flag],
 [AC_LANG_PUSH(C)
  AX_CHECK_COMPILE_FLAG([$1],
   [AC_LANG_PUSH(C++)
    AX_CHECK_COMPILE_FLAG([$1], [$2], [$3], [-Werror $4])
    AC_LANG_POP(C++)],
   [$3], [-Werror $4])
  AC_LANG_POP(C)])
AC_DEFUN([check_link_flag],
 [AX_CHECK_LINK_FLAG([$1], [$2], [$3], [-Werror $4])])

AC_ARG_ENABLE([hardening],
  [AS_HELP_STRING([--enable-hardening],
    [Enable compiler and linker options to frustrate memory corruption exploits @<:@yes@:>@])],
  [hardening="$enableval"],
  [hardening="yes"])

HARDEN_CFLAGS=""
HARDEN_LDFLAGS=""
AS_IF([test x"$hardening" != x"no"], [
  check_cc_cxx_flag([-fno-strict-overflow], [HARDEN_CFLAGS="$HARDEN_CFLAGS -fno-strict-overflow"])

#  This one will likely succeed, even on platforms where it does nothing.
  check_cc_cxx_flag([-D_FORTIFY_SOURCE=2], [HARDEN_CFLAGS="$HARDEN_CFLAGS -D_FORTIFY_SOURCE=2"])

  check_cc_cxx_flag([-fstack-protector-all],
   [check_link_flag([-fstack-protector-all],
     [HARDEN_CFLAGS="$HARDEN_CFLAGS -fstack-protector-all"
      check_cc_cxx_flag([-Wstack-protector], [HARDEN_CFLAGS="$HARDEN_CFLAGS -Wstack-protector"],
        [], [-fstack-protector-all])
      check_cc_cxx_flag([--param ssp-buffer-size=1], [HARDEN_CFLAGS="$HARDEN_CFLAGS --param ssp-buffer-size=1"],
        [], [-fstack-protector-all])])])

  # At the link step, we might want -pie (GCC) or -Wl,-pie (Clang on OS X)
  #
  # The linker checks also compile code, so we need to include -fPIE as well.
  check_cc_cxx_flag([-fPIE],
   [check_link_flag([-fPIE -pie],
     [HARDEN_CFLAGS="$HARDEN_CFLAGS -fPIE"
      HARDEN_LDFLAGS="$HARDEN_LDFLAGS -pie"],
     [check_link_flag([-fPIE -Wl,-pie],
       [HARDEN_CFLAGS="$HARDEN_CFLAGS -fPIE"
        HARDEN_LDFLAGS="$HARDEN_LDFLAGS -Wl,-pie"])])])

  check_link_flag([-Wl,-z,relro],
   [HARDEN_LDFLAGS="$HARDEN_LDFLAGS -Wl,-z,relro"
    check_link_flag([-Wl,-z,now], [HARDEN_LDFLAGS="$HARDEN_LDFLAGS -Wl,-z,now"])])])

AC_SUBST([HARDEN_CFLAGS])
AC_SUBST([HARDEN_LDFLAGS])

# End of flag tests.
CC="$saved_CC"
CXX="$saved_CXX"
LD="$saved_LD"
##### AUTOHARDEN END

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netinet/in.h stdlib.h string.h sys/param.h sys/socket.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 gettimeofday inet_ntoa memset socket strdup strerror strstr])

CORE_LIBS=""
AC_ARG_ENABLE(libev,
 [  --enable-libev=yes    specify if you want to compile Honeybrid with libev (EXPERIMENTAL)],
 [ if   test "x$enableval" = "xyes" ; then
	AC_CHECK_LIB(ev, ev_default_loop, [
		AC_DEFINE([HAVE_LIBEV], [1], [libev enabled])
		AC_MSG_NOTICE([Using libev for event handling])
		CORE_LIBS="$CORE_LIBS -lev"
	], [AC_MSG_NOTICE([Libev not found])])
   fi
 ])

AC_CHECK_LIB(nfnetlink, nfnl_fd, [], [AC_ERROR([libnfnetlink library is missing nfnl_fd, try to compile it from source!])])
AC_CHECK_LIB(netfilter_queue, [nfq_set_verdict2], 
	[CORE_LIBS="$CORE_LIBS -lnetfilter_queue"],
	[AC_ERROR([libnetfilter_queue is too old. Please upgrade it to 1.0.0 or later!])])
AC_CHECK_LIB(pcap, pcap_compile_nopcap, [CORE_LIBS="$CORE_LIBS -lpcap"], [AC_ERROR(PCAP library missing)])
AC_CHECK_LIB(dumbnet, addr_ntoa, [CORE_LIBS="$CORE_LIBS -ldumbnet"], [AC_ERROR(Dumbnet library missing)])
AC_SUBST(CORE_LIBS)

PKG_CHECK_MODULES([CRYPTO], [libcrypto], [
	AC_DEFINE([HAVE_CRYPTO], [1], [Crypto library found])
	], [])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.32 gthread-2.0])

AC_ARG_ENABLE(xmpp,
 [  --enable-xmpp=yes    specify if you want to compile Honeybrid with Dionaea-XMPP module],
 [ if   test "x$enableval" = "xyes" ; then
	PKG_CHECK_MODULES([XMPP], [gloox libxml++-2.6], [
		PKG_CHECK_MODULES([XML2], [libxml-2.0], [
			AC_MSG_NOTICE([Enabling XMPP listener and Dionaea module])
			AC_DEFINE([HAVE_XMPP], [1], [XMPP listener enabled])
			])
		],
		[AC_MSG_NOTICE([XMPP listener won't be compiled])])
		
		if test "x$XMPP_LIBS" != "x" -a "xXML2_LIBS" == "x"; then
			AC_ERROR(XML2 libs are missing)
		fi
		
		if test "x$XML2_LIBS" != "x" -a "x$XML2_CFLAGS" != "x"; then
			xmpp_listener="tools/dionaea_xmpp_listener"
		else
			xmpp_listener=""
		fi
		
		AC_SUBST(xmpp_listener)
		AC_CONFIG_FILES(tools/dionaea_xmpp_listener/Makefile)
	fi
 ])

AC_PATH_PROG(MYSQL_CONFIG, mysql_config)
	
if test "x$MYSQL_CONFIG" != "x"; then
	MYSQL_LIBS=$($MYSQL_CONFIG --libs)
	MYSQL_CFLAGS=$($MYSQL_CONFIG --cflags)

	AC_DEFINE(HAVE_MYSQL, 1, "Working MySQL library found")
	AC_MSG_NOTICE([Enabling MySQL logging])
	AC_SUBST(MYSQL_LIBS)
	AC_SUBST(MYSQL_CFLAGS)
else
	AC_MSG_NOTICE([No working MySQL library found, disabling MySQL logging])
fi
	
AC_ARG_ENABLE(doc,
 [  --enable-doc=yes 	specify if you want to compile the Doxygen documentation of the source code (require doxygen)],
 [ if   test "x$enableval" = "xyes" ; then
	doc="doc"
   else
	doc=""
   fi
   AC_SUBST(doc)
   AC_CONFIG_FILES(doc/Makefile)
 ])
	
AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(src/Makefile)
AC_OUTPUT
