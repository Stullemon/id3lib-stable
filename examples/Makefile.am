# Copyright (C) 1999 Scott Thomas Haug <sth2@cs.wustl.edu>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

LDADD_BASE =

if ID3_NEEDPOPT
popt_lib = $(top_builddir)/popt/libpopt.la
popt_include = -I$(top_srcdir)/popt
else
popt_lib = -lpopt
popt_include =
endif

if ID3_NEEDZLIB
zlib_lib = $(top_builddir)/zlib/src/libz.la
zlib_include = -I$(top_srcdir)/zlib/include
else
zlib_lib = -lz
zlib_include =
endif

LDADD =  $(top_builddir)/src/libid3.la $(popt_lib) $(zlib_lib)

INCLUDES = -I$(top_srcdir)/include $(popt_include)

bin_PROGRAMS            = id3info id3convert id3tag id3cp
check_PROGRAMS          = \
  id3simple               \
  testpic                 \
  testunicode             \
  testcompression         \
  testremove              \
  get_pic                 \
  findstr                 \
  findeng
id3cp_SOURCES           = demo_copy.cpp
id3info_SOURCES         = demo_info.cpp
id3convert_SOURCES      = demo_convert.cpp
id3tag_SOURCES          = demo_tag.cpp
id3simple_SOURCES       = demo_simple.c
testpic_SOURCES         = test_pic.cpp
testunicode_SOURCES     = test_unicode.cpp
testcompression_SOURCES = test_compression.cpp
testremove_SOURCES      = test_remove.cpp
get_pic_SOURCES         = get_pic.cpp
findeng_SOURCES         = findeng.cpp
findstr_SOURCES         = findstr.cpp


EXTRA_DIST =            \
  composer.jpg          \
  demo_main.cpp         \
  221-compressed.tag    \
  230-compressed.tag    \
  230-picture.tag       \
  230-unicode.tag       \
  230-syncedlyrics.tag  \
  thatspot.tag          \
  ozzy.tag              
