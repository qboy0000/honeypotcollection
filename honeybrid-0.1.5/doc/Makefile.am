# Copyright (C) 2009 Robin Berthier <robinb@umd.edu>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

PROJECT=honeybrid
DOCDIR=./
SRCDIR=../src/
GENERATE_HTML=YES
GENERATE_CHM=NO
HHC_PATH=""
GENERATE_CHI=NO
GENERATE_LATEX=NO
PAPER_SIZE=
GENERATE_RTF=NO
GENERATE_MAN=NO
GENERATE_XML=NO
PERL_PATH=/usr/bin/perl
HAVE_DOT=YES
DOT_PATH=/usr/bin/

EXTRA_DIST = doxygen.cfg

export PROJECT DOCDIR SRCDIR GENERATE_HTML GENERATE_CHM HHC_PATH GENERATE_CHI GENERATE_LATEX PAPER_SIZE GENERATE_RTF GENERATE_MAN GENERATE_XML PERL_PATH HAVE_DOT DOT_PATH

all:
	doxygen doxygen.cfg
