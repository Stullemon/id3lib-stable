# $Id: Makefile.am,v 1.10 2000/04/09 14:05:44 eldamitri Exp $
# 
# Copyright (C) 1999 Scott Thomas Haug <sth2@cs.wustl.edu>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

EXTRA_DIST =                    \
        reconf                  \
        config.h.win32          \
        config.h.win32.in 
SUBDIRS = . m4 doc src include prj zlib

INCLUDES = @ID3LIB_DEBUG_FLAGS@

config.h.win32: $(top_builddir)/config.status $(top_srcdir)/config.h.win32.in 
	cd $(top_builddir) && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status 

.PHONY: release snapshot

release: config.h.win32
	rm -rf .deps */.deps
	$(MAKE) distcheck
	GZIP=$(GZIP_ENV) $(TAR) zxf $(distdir).tar.gz
	cd $(distdir) && cp config.h.win32 config.h
	cd $(distdir) && zip -r ../$(distdir).zip *
	-rm -rf $(distdir)

snapshot: config.h.win32
	ss_distdir=$(PACKAGE)-`date +"%Y%m%d"`; \
	$(MAKE) distdir distdir=$$ss_distdir; \
	chmod -R a+r $$ss_distdir; \
	GZIP=$(GZIP_ENV) $(TAR) chozf $${ss_distdir}.tar.gz $$ss_distdir; \
	cd $$ss_distdir && cp config.h.win32 config.h && cd ..; \
	cd $$ss_distdir && zip -r ../$${ss_distdir}.zip * && cd ..; \
	rm -rf $$ss_distdir

# $Log: Makefile.am,v $
# Revision 1.10  2000/04/09 14:05:44  eldamitri
# (config.h.win32): Fixed bug in creating config.h.win32.
# (release): Added.  Creates id3lib-$(VERSION).tar.gz and
# id3lib-$(VERSION).zip.
# (snapshot): Added.  Created id3lib-$(DATE).tar.gz and
# id3lib-$(DATE).zip
#
# Revision 1.9  2000/04/08 04:47:38  eldamitri
# Updated for config.h.win32.
#
# Revision 1.8  2000/04/06 19:36:48  eldamitri
# * Makefile.am:
# Put in version requirement for automake (1.4).
#
# Revision 1.7  1999/12/03 14:10:11  scott
# (EXTRA_DIST): Added config.win32.
#
# Revision 1.6  1999/12/03 00:45:17  scott
# (SUBDIRS): Added prj and zlib
#
# Revision 1.5  1999/12/01 19:08:59  scott
# (SUBDIRS): Added "include".
#
# Revision 1.4  1999/11/30 19:54:08  scott
# (EXTRA_DIST): Removed ChangeLog, configure, HISTORY, and TODO.
# These are included in the distribution automatically.
#
