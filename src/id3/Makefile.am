# $Id: Makefile.am,v 1.8 1999/12/02 01:49:12 scott Exp $
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

LTVERSION = @lt_ver@:@lt_rev@:@lt_age@

INCLUDES = -I$(top_srcdir)/include/id3

the_sources =                   \
  dll_wrapper.cpp               \
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

lib_LTLIBRARIES   = libid3.la 

libid3_la_SOURCES = $(the_sources)
libid3_la_LIBADD  = -lz
libid3_la_LDFLAGS = -version-info $(LTVERSION)

# $Log: Makefile.am,v $
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
