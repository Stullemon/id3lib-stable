# $Id: Makefile.am,v 1.14 2000/04/21 19:55:09 eldamitri Exp $
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
	HISTORY                 \
        config.h.win32          \
        config.h.win32.in       \
	id3lib.spec             \
        id3lib.spec.in          \
        reconf
SUBDIRS = . m4 doc src include prj zlib id3com libprj examples

INCLUDES = @ID3LIB_DEBUG_FLAGS@

config.h.win32: $(top_builddir)/config.status $(top_srcdir)/config.h.win32.in 
	cd $(top_builddir) && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status

id3lib.spec: $(top_builddir)/config.status $(top_srcdir)/id3lib.spec.in 
	cd $(top_builddir) && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status

docsdistdir = $(PACKAGE)-doc-$(VERSION)

.PHONY: release snapshot docs-release docs

docs:
	-cd doc && $(MAKE) $(AM_MAKEFLAGS) $@

docs-release: docs
	-mv doc/$(docsdistdir).* .
	-cd examples && $(MAKE) $(AM_MAKEFLAGS) clean
	-mkdir $(docsdistdir)
	-cp -R examples           $(docsdistdir)
	-cp    doc/*.*            $(docsdistdir)
	-cp -R doc/@DOX_DIR_HTML@ $(docsdistdir)
	-find $(docsdistdir) -name "Makefile*" -exec rm -f {} \;
	-find $(docsdistdir) -name "*.mp3"     -exec rm -f {} \;
	GZIP=$(GZIP_ENV) $(TAR) zcf $(docsdistdir).tar.gz $(docsdistdir)
	-rm -rf $(docsdistdir)

release: config.h.win32 id3lib.spec
	-rm -rf .deps */.deps $(distdir).zip
	$(MAKE) $(AM_MAKEFLAGS) distcheck
	GZIP=$(GZIP_ENV) $(TAR) zxf $(distdir).tar.gz
	cd $(distdir) && cp config.h.win32 config.h
	cd $(distdir) && zip -r ../$(distdir).zip *
	cd $(distdir) && ./configure && $(MAKE) $(AM_MAKEFLAGS) docs-release
	mv $(distdir)/$(docsdistdir).* .
	-rm -rf $(distdir)

snapshot: config.h.win32
	ss_distdir=$(PACKAGE)-`date +"%Y%m%d"`; \
	$(MAKE) $(AM_MAKEFLAGS) distdir distdir=$$ss_distdir; \
	chmod -R a+r $$ss_distdir; \
	GZIP=$(GZIP_ENV) $(TAR) chozf $${ss_distdir}.tar.gz $$ss_distdir; \
	cd $$ss_distdir && cp config.h.win32 config.h && cd ..; \
	cd $$ss_distdir && zip -r ../$${ss_distdir}.zip * && cd ..; \
	rm -rf $$ss_distdir

# $Log: Makefile.am,v $
# Revision 1.14  2000/04/21 19:55:09  eldamitri
# (docs): Added for creating documentation from top directory
# (docs-release): Added for automating creation of documentation
# deliverables
# (release): Updated to create documentation release
#
# Revision 1.13  2000/04/19 02:03:31  eldamitri
# (SUBDIRS): Added 'examples'
#
# Revision 1.12  2000/04/18 05:16:55  eldamitri
# (EXTRA_DIST): Added HISTORY and id3lib.spec[.in]
# (id3lib.spec): Added rule for generating from id3lib.spec.in
# (release): Now depends on id3lib.spec
#
# Revision 1.11  2000/04/12 16:31:14  eldamitri
# (SUBDIRS): Added id3com and libprj
#
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
