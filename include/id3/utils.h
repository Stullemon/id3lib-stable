// $Id: utils.h,v 1.11 2000/09/30 22:10:28 eldamitri Exp $

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/

#ifndef _ID3LIB_UTILS_H_
#define _ID3LIB_UTILS_H_

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream.h>
#include "globals.h"
#include "strings.h"

namespace id3
{
#ifdef  MAXPATHLEN
#  define ID3_PATH_LENGTH   (MAXPATHLEN + 1)
#elif   defined (PATH_MAX)
#  define ID3_PATH_LENGTH   (PATH_MAX + 1)
#else   /* !MAXPATHLEN */
#  define ID3_PATH_LENGTH   (2048 + 1)
#endif  /* !MAXPATHLEN && !PATH_MAX */

  template<typename T>
  const T& min(const T& a, const T& b)
  {
    return (a < b) ? a : b;
  }

  template<typename T>
  const T& max(const T& a, const T& b)
  {
    return (b < a) ? a : b;
  }

  template<typename T>
  T abs(const T& a)
  {
    return (a < T(0)) ? -a : a;
  }

  size_t timeToSeconds(const char*, size_t);
  bool isCRLF(const char*, const char*);
  size_t CRLFtoLF(char *, size_t);

  void   removeTrailingSpaces(char*, size_t);
  id3::string removeTrailingSpaces(id3::string);
  
  uint32 parseNumber(const uchar *buffer, size_t size = sizeof(uint32));
  size_t renderNumber(uchar *buffer, uint32 val, size_t size = sizeof(uint32));
  id3::string renderNumber(uint32 val, size_t size = sizeof(uint32));
  
  void   mbstoucs(unicode_t *unicode, const char *ascii, const size_t len);
  void   ucstombs(char *ascii, const unicode_t *unicode, const size_t len);
  size_t ucslen(const unicode_t *unicode);
  void   ucscpy(unicode_t *dest, const unicode_t *src);
  void   ucsncpy(unicode_t *dest, const unicode_t *src, size_t len);
  int    ucscmp(const unicode_t *s1, const unicode_t *s2);
  int    ucsncmp(const unicode_t *s1, const unicode_t *s2, size_t len);

  // these can be utility functions
  size_t     getUnSyncSize(uchar *, size_t);
  void       unsync(uchar *, size_t, const uchar *, size_t);
  size_t     resync(uchar *, size_t);

  // file utils
  size_t getFileSize(fstream&);
  size_t getFileSize(ifstream&);
  size_t getFileSize(ofstream&);
  ID3_Err createFile(const char*, fstream&);
  ID3_Err openWritableFile(const char*, fstream&);
  ID3_Err openWritableFile(const char*, ofstream&);
  ID3_Err openReadableFile(const char*, fstream&);
  ID3_Err openReadableFile(const char*, ifstream&);

}
  
#endif /* _ID3LIB_UTILS_H_ */
