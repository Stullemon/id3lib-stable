// $Id: utils.cpp,v 1.11 2000/10/09 04:33:03 eldamitri Exp $

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

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include "utils.h"

using namespace dami;

size_t dami::renderNumber(uchar *buffer, uint32 val, size_t size)
{
  uint32 num = val;
  for (size_t i = 0; i < size; i++)
  {
    buffer[size - i - 1] = (uchar)(num & MASK8);
    num >>= 8;
  }
  return size;
}

String dami::renderNumber(uint32 val, size_t size)
{
  String str(size, '\0');
  uint32 num = val;
  for (size_t i = 0; i < size; i++)
  {
    str[size - i - 1] = (uchar)(num & MASK8);
    num >>= 8;
  }
  return str;
}

// converts an ASCII string into a Unicode one

void dami::mbstoucs(unicode_t *unicode, const char *ascii, size_t len)
{
  if (NULL != ascii && NULL != unicode)
  {
    for (index_t i = 0; i < len; i++)
    {
      unicode[i] = ascii[i] & 0xFF;
    }
  }
}

// converts a Unicode string into ASCII

void dami::ucstombs(char *ascii, const unicode_t *unicode, size_t len)
{
  if (NULL != unicode && NULL != ascii)
  {
    for (index_t i = 0; i < len; i++)
    {
      ascii[i] = unicode[i] & 0x00FF;
    }
  }
}

size_t dami::ucslen(const unicode_t *unicode)
{
  if (NULL != unicode)
  {
    for (size_t size = 0; true; size++)
    {
      if (NULL_UNICODE == unicode[size])
      {
        return size;
      }
    }
  }
  return 0;
}

void dami::ucscpy(unicode_t *dest, const unicode_t *src)
{
  if (NULL != dest && NULL != src)
  {
    size_t i;
    for (i = 0; NULL_UNICODE != src[i]; i++)
    {
      dest[i] = src[i];
    }
    dest[i] = NULL_UNICODE;
  }
}

void dami::ucsncpy(unicode_t *dest, const unicode_t *src, size_t len)
{
  if (NULL != dest && NULL != src)
  {
    size_t i;
    for (i = 0; i < len && NULL_UNICODE != src[i]; i++)
    {
      dest[i] = src[i];
    }
    for (; i < len; i++)
    {
      dest[i] = NULL_UNICODE;
    }
  }
}

int dami::ucscmp(const unicode_t *s1, const unicode_t *s2)
{
  return ucsncmp(s1, s2, (size_t) -1);
}

int dami::ucsncmp(const unicode_t *s1, const unicode_t *s2, size_t len)
{
  if (NULL == s1 && NULL == s2)
  {
    return  0;
  }
  if (NULL == s1)
  {
    return  1;
  }
  if (NULL == s2)
  {
    return -1;
  }
  for (size_t i = 0; true; i++)
  {
    if ((NULL_UNICODE == s1[i]) || (NULL_UNICODE == s2[i]) ||
        (s1[i] != s2[i]) || (i + 1 == len))
    {
      return s2[i] - s1[i];
    }
  }
}

namespace
{
  bool exists(const char *name)
  {
    ifstream file(name, ios::nocreate);
    return file.is_open() != 0;
  }
};

ID3_Err dami::createFile(const char* name, fstream& file)
{
  if (file.is_open())
  {
    file.close();
  }
    
  file.open(name, ios::out | ios::binary | ios::trunc);
  if (!file)
  {
    return ID3E_ReadOnly;
  }
    
  return ID3E_NoError;
}

size_t dami::getFileSize(fstream& file)
{
  size_t size = 0;
  if (file.is_open())
  {
    streamoff curpos = file.tellg();
    file.seekg(0, ios::end);
    size = file.tellg();
    file.seekg(curpos);
  }
  return size;
}

size_t dami::getFileSize(ifstream& file)
{
  size_t size = 0;
  if (file.is_open())
  {
    streamoff curpos = file.tellg();
    file.seekg(0, ios::end);
    size = file.tellg();
    file.seekg(curpos);
  }
  return size;
}

size_t dami::getFileSize(ofstream& file)
{
  size_t size = 0;
  if (file.is_open())
  {
    streamoff curpos = file.tellp();
    file.seekp(0, ios::end);
    size = file.tellp();
    file.seekp(curpos);
  }
  return size;
}

ID3_Err dami::openWritableFile(const char* name, fstream& file)
{
  if (!exists(name))
  {
    return ID3E_NoFile;
  }
    
  if (file.is_open())
  {
    file.close();
  }
  file.open(name, ios::in | ios::out | ios::binary | ios::nocreate);
  if (!file)
  {
    return ID3E_ReadOnly;
  }
    
  return ID3E_NoError;
}

ID3_Err dami::openWritableFile(const char* name, ofstream& file)
{
  if (!exists(name))
  {
    return ID3E_NoFile;
  }
    
  if (file.is_open())
  {
    file.close();
  }
  file.open(name, ios::in | ios::out | ios::binary | ios::nocreate);
  if (!file)
  {
    return ID3E_ReadOnly;
  }
    
  return ID3E_NoError;
}

ID3_Err dami::openReadableFile(const char* name, fstream& file)
{
  if (file.is_open())
  {
    file.close();
  }
  file.open(name, ios::in | ios::binary | ios::nocreate);
  if (!file)
  {
    return ID3E_NoFile;
  }
    
  return ID3E_NoError;
}

ID3_Err dami::openReadableFile(const char* name, ifstream& file)
{
  if (file.is_open())
  {
    file.close();
  }
  file.open(name, ios::in | ios::binary | ios::nocreate);
  if (!file)
  {
    return ID3E_NoFile;
  }
    
  return ID3E_NoError;
}

String dami::toString(uint32 val)
{
  if (val == 0)
  {
    return "0";
  }
  String text;
  while (val > 0)
  {
    String tmp;
    char ch = (val % 10) + '0';
    tmp += ch;
    text = tmp + text;
    val /= 10;
  }
  return text;
}
