# Copyright (C) 1999 Scott Thomas Haug <sth2@cs.wustl.edu>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

LTVERSION = 3:6:0

INCLUDES = -I$(srcdir)/..

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
  tag_sync.cpp

the_headers =                   \
  dll.h                         \
  error.h                       \
  externals.h                   \
  field.h                       \
  frame.h                       \
  header.h                      \
  header_frame.h                \
  header_tag.h                  \
  int28.h                       \
  misc_support.h                \
  tag.h                         \
  types.h                       \
  version.h

lib_LTLIBRARIES   = libid3.la 

libid3_la_SOURCES = $(the_sources)
# CXXFLAGS = -Wall
libid3_la_LIBADD  = -lz
libid3_la_LDFLAGS = -version-info $(LTVERSION)

id3includedir      = $(includedir)/id3
id3include_HEADERS = $(the_headers)

EXTRA_DIST = ChangeLog