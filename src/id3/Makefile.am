# $Id: Makefile.am,v 1.10 2000/04/07 04:47:55 eldamitri Exp $
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

LT_VERSION = $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

INCLUDES = @ID3LIB_DEBUG_FLAGS@ -I$(top_srcdir)/include/id3

the_sources =                   \
  error.cpp                     \
  field.cpp                     \
  field_binary.cpp              \
  field_integer.cpp             \
  field_string_ascii.cpp        \
  field_string_unicode.cpp      \
  frame.cpp                     \
  frame_parse.cpp               \
  frame_render.cpp              \
  header.cpp                    \
  header_frame.cpp              \
  header_tag.cpp                \
  int28.cpp                     \
  misc_support.cpp              \
  tag.cpp                       \
  tag_file.cpp                  \
  tag_find.cpp                  \
  tag_parse.cpp                 \
  tag_parse_lyrics3.cpp         \
  tag_parse_v1.cpp              \
  tag_render.cpp                \
  tag_sync.cpp                  \
  version.cpp                   

EXTRA_DIST =                    \
  dll_wrapper.cpp               


lib_LTLIBRARIES   = libid3.la 

libid3_la_SOURCES = $(the_sources)
libid3_la_LIBADD  = -lz
libid3_la_LDFLAGS = \
  -version-info $(LT_VERSION) \
  -release $(LT_RELEASE) \
  -export-dynamic

# $Log: Makefile.am,v $
# Revision 1.10  2000/04/07 04:47:55  eldamitri
# Updated versioning macros, ala glib (http://www.gtk.org).  Also added a
# few more tests.
#
# Revision 1.9  2000/01/04 16:33:26  eldamitri
# (the_sources): Moved dll_wrapper.cpp to EXTRA_DIST.
#
# Revision 1.8  1999/12/02 01:49:12  scott
# Removed EXTRA_DIST (no more ChangeLog)
#
# Revision 1.7  1999/12/01 18:03:28  scott
# (INCLUDES): Now points to new, toplevel include directory.
# (the_headers, id3includedir, id3include_HEADERS): Removed.  Headers are
# no longer in this directory.
#
# Revision 1.6  1999/11/30 20:08:16  scott
# (the_headers): Added sized_headers.h
# (the_sources): Added version.cpp
# (LTVERSION): Updated to make use of configure.in variables.
#
