# Copyright (C) 1999 Scott Thomas Haug <sth2@cs.wustl.edu>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES = -I$(top_srcdir)/include
LDADD = $(top_builddir)/src/id3/libid3.la

check_PROGRAMS = id3info id3convert
id3info_SOURCES = demo_info.cpp
id3convert_SOURCES = demo_convert.cpp

EXTRA_DIST =            \
  demo_main.cpp         \
  jules.mp3             \
  jules-badtag.mp3      \
  jules-goodtag.mp3     \
  221-compressed.tag    \
  230-compressed.tag    \
  230-picture.tag       \
  230-unicode.tag       \
  readme.txt
