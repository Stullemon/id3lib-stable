// $Id: field_string_unicode.cpp,v 1.11 1999/12/09 03:32:14 scott Exp $
// 
// The authors have released ID3Lib as Public Domain (PD) and claim no
// copyright, patent or other intellectual property protection in this work.
// This means that it may be modified, redistributed and used in commercial
// and non-commercial software and hardware without restrictions.  ID3Lib is
// distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
// express or implied.
// 
// The ID3Lib authors encourage improvements and optimisations to be sent to
// the ID3Lib coordinator, currently Scott Haug (sth2@cs.wustl.edu).  Approved
// submissions may be altered, and will be included and released under these
// terms.

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include "field.h"
#include "misc_support.h"

// this function is another way of using Set()

ID3_Field& ID3_Field::operator= (const unicode_t *string)
{
  Set(string);
  
  return *this;
}


// this is Set()

void ID3_Field::Set(const unicode_t *string)
{
  luint nBytes =
    (-1 == __lFixedLength) ? ucslen(string) : __lFixedLength;
  
  // we can simply increment the nBytes count here because we just pilfer
  // the NULL which is present in the string which was passed to us
  if (__ulFlags & ID3FF_NULL)
  {
    nBytes++;
  }
    
  // doubling the nBytes because Unicode is twice the size of ASCII
  nBytes *= sizeof(unicode_t);
  
  Set((uchar *) string, nBytes);
  
  __eType = ID3FTY_UNICODESTRING;
  __bHasChanged = true;
  
  return ;
}


void ID3_Field::Add(const unicode_t *string)
{
  if (NULL == __sData)
  {
    Set(string);
  }
  else
  {
    unicode_t *uBuffer = (unicode_t *) __sData;

    // +1 is for the NULL at the end and the other +1 is for the list divider
    size_t newLen = ucslen(string) + ucslen(uBuffer) + 1 + 1;
    
    unicode_t *temp = new unicode_t[newLen];
    if (NULL == temp)
    {
      ID3_THROW(ID3E_NoMemory);
    }

    ucscpy(temp, uBuffer);

    // I use the value 1 as a divider because then I can change it to either a
    // '/' or a NULL at render time.  This allows easy use of these functions
    // for text lists or in the IPLS frame
    temp[ucslen(uBuffer)] = L'\001';
    ucscpy(&temp[ucslen(uBuffer) + 1], string);
    temp[newLen - 1] = NULL_UNICODE;
      
    Set(temp);
      
    delete [] temp;
  }
  
  return ;
}


// this is Get()

luint ID3_Field::Get(unicode_t *buffer, const luint maxChars, const luint itemNum)
{
  luint charsUsed = 0;
  
  // check to see if there is a string in the frame to copy before we even try
  if (NULL != __sData)
  {
    lsint nullOffset = 0;
    
    if (__ulFlags & ID3FF_NULL)
    {
      nullOffset = -1;
    }
      
    // first we must find which element is being sought to make sure it exists
    // before we try to get it
    if (itemNum <= GetNumTextItems() && itemNum > 0)
    {
      unicode_t *source = (unicode_t *) __sData;
      luint posn = 0;
      luint sourceLen = 0;
      luint curItemNum = 1;
      
      // now we find that element and set the souvre pointer
      while (curItemNum < itemNum)
      {
        while (*source != L'\001' && *source != L'\0' && posn <
               ((__ulSize / sizeof(unicode_t)) + nullOffset))
        {
          source++, posn++;
        }
          
        source++;
        curItemNum++;
      }
      
      // now that we are positioned at the first character of the string we
      // want, find the end of it
      while (source[sourceLen] != L'\001' && source[sourceLen] != L'\0' &&
             posn <((__ulSize / sizeof(unicode_t) + nullOffset)))
      {
        sourceLen++, posn++;
      }
        
      if (NULL == buffer)
      {
        ID3_THROW(ID3E_NoBuffer);
      }

      luint actualChars = MIN(maxChars, sourceLen);
        
      ucsncpy(buffer, source, actualChars);
      if (actualChars < maxChars)
      {
        buffer[actualChars] = L'\0';
      }
      charsUsed = actualChars;
    }
  }
  
  return charsUsed;
}


luint ID3_Field::GetNumTextItems(void)
{
  luint numItems = 0;
  
  if (NULL != __sData)
  {
    luint posn = 0;
    
    numItems++;
    
    while (posn < __ulSize)
    {
      if (__sData[posn++] == L'\001')
      {
        numItems++;
      }
    }
  }
  
  return numItems;
}


luint ID3_Field::ParseUnicodeString(const uchar *buffer, const luint posn, const luint buffSize)
{
  luint nBytes = 0;
  unicode_t *temp = NULL;
  if (__lFixedLength != -1)
  {
    nBytes = __lFixedLength;
  }
  else
  {
    if (__ulFlags & ID3FF_NULL)
    {
      while ((posn + nBytes) < buffSize &&
             !(buffer[posn + nBytes] == 0 && 
               buffer[posn + nBytes + 1] == 0))
      {
        nBytes += sizeof(unicode_t);
      }
    }
    else
    {
      nBytes = buffSize - posn;
    }
  }
  
  if (nBytes > 0)
  {
    // Sanity check our indices and sizes before we start copying memory
    if ((nBytes > buffSize) || (posn + nBytes > buffSize))
    {
      ID3_THROW_DESC(ID3E_BadData, "field information invalid");
    }

    temp = new unicode_t[(nBytes / sizeof(unicode_t)) + 1];
    if (NULL == temp)
    {
      ID3_THROW(ID3E_NoMemory);
    }

    luint loc = 0;

    memcpy(temp, &buffer[posn], nBytes);
    temp[nBytes / sizeof(unicode_t)] = NULL_UNICODE;
      
    // if there is a BOM, skip past it and check to see if we need to swap
    // the byte order around
    if (temp[0] == 0xFEFF || temp[0] == 0xFFFE)
    {
      loc++;
        
      // if we need to swap the byte order
      /* TODO: Determine if this the correct check to make sure bytes should
         be swapped.  For example, the example tag 230-unicode.tag (found in 
         the distrubitution) has two unicode sections, each that begin with
         the FEFF magic number.  Each unicode character is, as usual, two
         bytes.  The first byte is the ascii equivalent; the second is null.
         Is this the "correct" encoding?  When a little-endian parses each of
         those characters, the bytes are swapped, so they essentially end up
         as the ascii equivalent automatically.  The FEFF magic number is also
         swapped, so the number is evaluated as FFFE.  The original code below
         forced byteswapping if the value of the first unicode character was
         not equal to 0xFEFF.  This doesn't work for a little-endian machine,
         though, since, as the rest of the code now stands, swapping the bytes
         will not create a correct parse.  Therefore, the code swaps bytes
         only when the value is equal to FEFF.
      */
      if (temp[0] == 0xFEFF)
      {
        for (int i = loc; i < (int) ucslen(temp); i++)
        {
          uchar
            u1 = ((uchar *)(&temp[i]))[0],
            u2 = ((uchar *)(&temp[i]))[1];
          temp[i] = (u1 << 8) | u2;
        }
      }
    }
      
    Set(&temp[loc]);
      
    delete [] temp;
  }
  
  if (__ulFlags & ID3FF_NULL)
  {
    nBytes += sizeof(unicode_t);
  }
    
  __bHasChanged = false;
  
  return nBytes;
}


luint ID3_Field::RenderUnicodeString(uchar *buffer)
{
  luint nBytes = 0;
  
  nBytes = BinSize();
  
  if (NULL != __sData && __ulSize && nBytes)
  {
    luint i;
    unicode_t *ourString = (unicode_t *) & buffer[sizeof(unicode_t)];
    
    // we render at sizeof(unicode_t) bytes into the buffer because we make
    // room for the Unicode BOM
    memcpy(&buffer[sizeof(unicode_t)], (uchar *) __sData, 
           nBytes - sizeof(unicode_t));
    
    // now we convert the internal dividers to what they are supposed to be
    for (i = 0; i < nBytes; i++)
    {
      if (ourString[i] == 1)
      {
        unicode_t sub = L'/';
        
        if (__ulFlags & ID3FF_NULLDIVIDE)
        {
          sub = L'\0';
        }
          
        ourString[i] = sub;
      }
    }
  }
  
  if (nBytes)
  {
    // render the BOM
    unicode_t *BOM = (unicode_t *) buffer;
    BOM[0] = 0xFEFF;
  }
  
  if (nBytes == sizeof(unicode_t) && (__ulFlags & ID3FF_NULL))
  {
    for (size_t i = 0; i < sizeof(unicode_t); i++)
    {
      buffer[i] = 0;
    }
  }
    
  __bHasChanged = false;
  
  return nBytes;
}

// $Log: field_string_unicode.cpp,v $
// Revision 1.11  1999/12/09 03:32:14  scott
// (Add): Cleaned up and restructured code to make for easier reading.
// Also fixed a bug in adding strings to an involved people list frame.
//
// Revision 1.10  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.9  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.8  1999/11/29 18:15:19  scott
// (): Updated methods to use unicode_t instead of wchar_t.  unicode_t is
// defined in globals.h.  The type unicode_t is more likely to be two
// bytes on every platform.  Updated calls to unicode methods to more
// general methods defined in misc_support.cpp.
// (ParseUnicodeString): Fixed a bug (?) to make unicode parsing work.
// Not really sure if this is the source of the bug, or if there is a
// more pervasive misunderstanding of the format of unicode.  Need to
// check on this.
//
// Revision 1.7  1999/11/19 17:49:39  scott
// (operator=): Updated interface to make parameters const.
// (Set): Updated interface to make parameters const.  Bug fix for fixed
// length strings.  Minor variable renaming.
// (Add): Updated interface to make parameters const.
// (Get): Updated interface to make parameters const.  Fixed bug which
// would add a null character to the string regardless of the length of
// string requested.  Now only adds a null when the actual length of the
// string is less than the maxium number of characters requested.
//
// Revision 1.6  1999/11/16 22:50:29  scott
// * field_string_unicode.cpp
// (ParseUnicodeString): Put in sanity check for indices so that
// memcpy doesn't go out of bounds. Made unicode code more specific
// to the type of data storing unicode characters.  This fix doesn't
// work if the type is different than two bytes in size.  Need to fix
// so that the type is guaranteed to be two bytes.
// (RenderUnicodeString): Made unicode code more specific to the type
// of data storing unicode characters.
//
// Revision 1.5  1999/11/15 20:16:33  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
