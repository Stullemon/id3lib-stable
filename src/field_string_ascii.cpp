// $Id: field_string_ascii.cpp,v 1.15 2000/09/14 22:11:08 eldamitri Exp $

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

#include <string.h>
#include <stdlib.h>
#include "field.h"
#include "utils.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

/** \fn ID3_Field& ID3_Field::operator=(const char* data)
 ** \brief Shortcut for the Set operator.
 ** \param data The string to assign to this field
 ** \sa Set(const char*)
 **/

/** \brief Copies the supplied string to the field.
 ** You may dispose of the source string after a call to this method.
 ** \code
 **   myFrame.Field(ID3FN_TEXT).Set("ID3Lib is very cool!");
 ** \endcode
 **/
size_t ID3_Field::Set_i(const char* string, size_t size)
{
  this->Clear();
  this->SetEncoding(ID3TE_ASCII);
  if (_chars == 0 && size > 0)
  {
    _chars = size;
    _ascii = new char[_chars];
  }
  ::memcpy(_ascii, string, MIN(_chars, size));
  if (size < _chars)
  {
    ::memset(_ascii + size, '\0', _chars - size);
  }
  _changed = true;

  if (string == NULL || size == 0)
  {
    _num_items = 0;
  }
  else
  {
    _num_items = 1;
  }

  return MIN(_chars, size);
}

size_t ID3_Field::Set(const char *string)
{
  size_t len = 0;
  if (this->GetType() == ID3FTY_TEXTSTRING)
  {
    if (string == NULL)
    {
      len = this->Set_i(string, 0);
    }
    else
    {
      len = this->Set_i(string, strlen(string));
    }
  }
  return len;
}


/** For fields which support this feature, adds a string to the list of
 ** strings currently in the field.
 ** 
 ** This is useful for using id3v2 frames such as the involved people list,
 ** composer, and part of set.  You can use the GetNumTextItems() method to
 ** find out how many such items are in a list.
 ** 
 ** \code
 **   myFrame.Field(ID3FN_TEXT).Add("this is a test");
 ** \endcode
 ** 
 ** \param string The string to add to the field
 **/
size_t ID3_Field::Add_i(const char *str, size_t strLen)
{
  size_t len = 0;  // how much of str we copied into this field (max is strLen)
  if (this->GetNumTextItems() == 0)
  {
    // there aren't any text items in the field so just assign the string to
    // the field
    len = this->Set_i(str, strLen);
  }
  else
  {
    this->SetEncoding(ID3TE_ASCII);
    
    char* oldStr = _ascii;
    size_t oldLen = this->Size();
    //ASSERT(oldLen > 0);
    if (_fixed_length > 0)
    {
      _chars = _fixed_length;
    }
    else
    {
      _chars = oldLen + 1 + strLen;
    }
    
    if (oldLen + 1 >= _chars)
    {
      // our new length isn't any bigger than our old length, so there's 
      // nothing to copy.  set the null pointer, if possible, and set the
      // length of what we copied to 0
      _chars = oldLen;
      len = 0;
    }
    else
    {
      // our new length is bigger than our old, so we can copy some (possibly
      // all) of the bytes from str into this field
      len = _chars - oldLen - 1;
      // ASSERT(len > 0);
      _ascii = new char[_chars];
      ::memcpy(_ascii, oldStr, oldLen);
      delete [] oldStr;
      _ascii[oldLen] = '\0';
      ::memcpy(&_ascii[oldLen + 1], str, len);
      _num_items++;
    }
  }

  return len;
}

size_t ID3_Field::Add(const char *str)
{
  size_t len = 0;
  if (this->GetType() == ID3FTY_TEXTSTRING)
  {
    if (NULL == str)
    {
      len = this->Set_i(str, 0);
    }
    else
    {
      len = this->Add_i(str, ::strlen(str));
    }
  }
  return len;
}

// the ::Get() function for ASCII

/** Copies the contents of the field into the supplied buffer, up to the
 ** number of characters specified; for fields with multiple entries, the
 ** optional third parameter indicates which of the fields to retrieve.
 ** 
 ** The third parameter is useful when using text lists (see Add(const char*)
 ** for more details).  The default value for this third parameter is 1,
 ** which returns the entire string if the field contains only one item.
 ** 
 ** It returns the number of characters (not bytes necessarily, and not
 ** including any NULL terminator) of the supplied buffer that are now used.
 ** 
 ** \code
 **   char myBuffer[1024];
 **   size_t charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024);
 ** \endcode
 ** 
 ** It fills the buffer with as much data from the field as is present in the
 ** field, or as large as the buffer, whichever is smaller.
 ** 
 ** \code
 **   char myBuffer[1024];
 **   size_t charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024, 3);
 ** \endcode
 ** 
 ** This fills the buffer with up to the first 1024 characters from the third
 ** element of the text list.
 ** 
 ** \sa Add(const char*)
 **/
size_t ID3_Field::Get(char* buffer, size_t maxLength) const
{
  size_t length = 0;
  if (this->GetType() == ID3FTY_TEXTSTRING && 
      this->GetEncoding() == ID3TE_ASCII &&
      buffer != NULL && maxLength > 0)
  {
    size_t size = this->Size();
    length = MIN(maxLength, size);
    ::memcpy(buffer, _ascii, length);
    if (length < maxLength)
    {
      buffer[length] = '\0';
    }
  }

  return length;
}

size_t ID3_Field::Get(char* buffer,     ///< Where to copy the data
                      size_t maxLength, ///< Max number of characters to copy
                      index_t itemNum   ///< The item number to retrieve
                      ) const
{
  size_t length = 0;
  size_t total_items = this->GetNumTextItems();
  if (this->GetType() == ID3FTY_TEXTSTRING && 
      this->GetEncoding() == ID3TE_ASCII &&
      buffer != NULL && maxLength > 0 && itemNum < total_items)
  {
    char* cur = _ascii;
    char* end = _ascii + _chars;
    size_t num = 0;
    while (cur < end && num < itemNum)
    {
      // the last item in the list probably doesn't have a null terminator, so
      // we must anticipate so that we don't determine its length with strlen
      if (num == total_items - 1)
      {
        break;
      }
      cur += strlen(cur) + 1;
      num++;
    }
    if (cur < end)
    {
      // the last item in the list probably doesn't have a null terminator, so
      // we must anticipate so that we don't determine its length with strlen
      if (itemNum == total_items - 1)
      {
        length = end - cur;
      }
      else
      {
        length = strlen(cur);
      }
      ::memcpy(buffer, cur, length);
      if (length < maxLength)
      {
        buffer[length] = '\0';
      }
    }
  }

  return length;
}



size_t 
ID3_Field::ParseASCIIString(const uchar *buffer, size_t nSize)
{
  size_t nChars = 0;
  const char* ascii = (const char*) buffer;
  this->Clear();
  
  if (this->Size() > 0)
  {
    // The string is of fixed length
    nChars = this->Size();
  }
  else if (!(_flags & ID3FF_CSTR) || (_flags & ID3FF_LIST))
  {
    // If the string isn't null-terminated or if it is null divided, we're
    // assured this is the last field of of the frame, and we can claim the
    // remaining bytes for ourselves
    nChars = nSize;
  }
  else
  {
    // it would be nice to use strlen to find nChars, but the buffer might
    // be invalid, so there might not be a null byte to mark the end of the
    // string
    while (nChars < nSize && ascii[nChars] != '\0')
    {
      nChars++;
    }
  }

  if (0 == nChars)
  {
    nChars = this->Set_i(static_cast<const char *>(NULL), 0);
  }
  // This check needs to come before the check for ID3FF_CSTR
  else if (_flags & ID3FF_LIST)
  {
    const char* cur = ascii;
    const char* end = cur + nChars;
    while (cur < end)
    {
      size_t length = strlen(cur);
      // sanity check!
      if (length + cur > end)
      {
        length = end - cur;
      }
      cur += this->Add_i(cur, length) + 1;
    }
    nChars = cur - ascii;
  }
  else
  {
    // Sanity check our indices and sizes before we start copying memory
    if (nChars > nSize)
    {
      nChars = nSize;
    }

    nChars = this->Set_i(ascii, nChars);
  }
  
  if (_flags & ID3FF_CSTR)
  {
    nChars++;
  }
    
  _changed = false;
  
  return MIN(nChars, nSize);
}

size_t ID3_Field::RenderString(uchar *buffer) const
{
  size_t nBytes = 0;
  if (this->GetEncoding() == ID3TE_ASCII)
  {
    nBytes = this->Size();
    if (nBytes > 0)
    {
      ::memcpy(buffer, _ascii, nBytes);
    }
    if (_flags & ID3FF_CSTR)
    {
      buffer[nBytes] = '\0';
      nBytes++;
    }
  }
  else if (this->GetEncoding() == ID3TE_UNICODE)
  {
    size_t nChars = this->Size();
    unicode_t* unicode = (unicode_t *) buffer;
    if (nChars > 0)
    {
      // BOM
      *unicode = 0xFEFF;
      unicode++;
      ::memcpy((void *) unicode, (void *) _unicode, nChars * 2);
      nChars++;
    }
    if (_flags & ID3FF_CSTR)
    {
      unicode[nChars++] = NULL_UNICODE;
    }
    nBytes = nChars * 2;
  }
  _changed = false;
  return nBytes;
}
