// $Id: field_string_unicode.cpp,v 1.7 1999/11/19 17:49:39 scott Exp $

//  The authors have released ID3Lib as Public Domain (PD) and claim no
//  copyright, patent or other intellectual property protection in this work.
//  This means that it may be modified, redistributed and used in commercial
//  and non-commercial software and hardware without restrictions.  ID3Lib is
//  distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
//  express or implied.
//
//  The ID3Lib authors encourage improvements and optimisations to be sent to
//  the ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
//  submissions may be altered, and will be included and released under these
//  terms.
//
//  Mon Nov 23 18:34:01 1998

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <id3/field.h>


// this function is another way of using Set()

ID3_Field& ID3_Field::operator= (const wchar_t *string)
{
  Set(string);
  
  return *this;
}


// this is Set()

void ID3_Field::Set(const wchar_t *string)
{
  luint nBytes = (-1 == __lFixedLength) ? wcslen(string) : __lFixedLength;
  
  // we can simply increment the nBytes count here because we just pilfer
  // the NULL which is present in the string which was passed to us
  if (__ulFlags & ID3FF_NULL)
  {
    nBytes++;
  }
    
  // doubling the nBytes because Unicode is twice the size of ASCII
  nBytes *= sizeof(wchar_t);
  
  Set((uchar *) string, nBytes);
  
  __eType = ID3FTY_UNICODESTRING;
  __bHasChanged = true;
  
  return ;
}


void ID3_Field::Add(const wchar_t *string)
{
  if (NULL == __sData)
    Set(string);
  else
  {
    wchar_t *temp;
    luint newLen;
    lsint nullOffset = 0;
    
    // if there is a NULL in this string, set this offset so that we ignore it
    // in string size calculations
    if (__ulFlags & ID3FF_NULL)
      nullOffset = -1;
      
    // +1 is for the NULL at the end and the other +1 is for the list divider
    newLen = 1 + (__ulSize / sizeof(wchar_t)) + wcslen(string) + 1 + nullOffset;
    
    // I use the value 1 as a divider because then I can change it to either a
    // '/' or a NULL at render time.  This allows easy use of these functions
    // for text lists or in the IPLS frame
    
    temp = new wchar_t[newLen];
    if (NULL == temp)
      ID3_THROW(ID3E_NoMemory);

    wcscpy(temp, (wchar_t *) __sData);
    temp[(__ulSize / sizeof(wchar_t)) + nullOffset] = L'\001';
    wcscpy(&temp[(__ulSize / sizeof(wchar_t)) + 1 + nullOffset], string);
      
    Set(temp);
      
    delete [] temp;
  }
  
  return ;
}


// this is Get()

luint ID3_Field::Get(wchar_t *buffer, const luint maxChars, const luint itemNum)
{
  luint charsUsed = 0;
  
  // check to see if there is a string in the frame to copy before we even try
  if (NULL != __sData)
  {
    lsint nullOffset = 0;
    
    if (__ulFlags & ID3FF_NULL)
      nullOffset = -1;
      
    // first we must find which element is being sought to make sure it exists
    // before we try to get it
    if (itemNum <= GetNumTextItems() && itemNum > 0)
    {
      wchar_t *source = (wchar_t *) __sData;
      luint posn = 0;
      luint sourceLen = 0;
      luint curItemNum = 1;
      
      // now we find that element and set the souvre pointer
      while (curItemNum < itemNum)
      {
        while (*source != L'\001' && *source != L'\0' && posn <
               ((__ulSize / sizeof(wchar_t)) + nullOffset))
          source++, posn++;
          
        source++;
        curItemNum++;
      }
      
      // now that we are positioned at the first character of the string we
      // want, find the end of it
      while (source[sourceLen] != L'\001' && source[sourceLen] != L'\0' &&
             posn <((__ulSize / sizeof(wchar_t) + nullOffset)))
        sourceLen++, posn++;
        
      if (NULL == buffer)
        ID3_THROW(ID3E_NoBuffer);

      luint actualChars = MIN(maxChars, sourceLen);
        
      wcsncpy(buffer, source, actualChars);
      if (actualChars < maxChars)
        buffer[actualChars] = L'\0';
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
      if (__sData[posn++] == L'\001')
        numItems++;
  }
  
  return numItems;
}


luint ID3_Field::ParseUnicodeString(const uchar *buffer, const luint posn, const luint buffSize)
{
  luint nBytes = 0;
  wchar_t *temp = NULL;
  
  if (__lFixedLength != -1)
    nBytes = __lFixedLength;
  else
  {
    if (__ulFlags & ID3FF_NULL)
      while ((posn + nBytes) < buffSize &&
             !(buffer[posn + nBytes] == 0 && 
               buffer[posn + nBytes + 1] == 0))
        nBytes += sizeof(wchar_t);
    else
      nBytes = buffSize - posn;
  }
  
  if (nBytes > 0)
  {
    // Sanity check our indices and sizes before we start copying memory
    if ((nBytes > buffSize) || (posn + nBytes > buffSize))
    {
      ID3_THROW_DESC(ID3E_BadData, "field information invalid");
    }

    temp = new wchar_t[(nBytes / sizeof(wchar_t)) + 1];
    if (NULL == temp)
      ID3_THROW(ID3E_NoMemory);

    luint loc = 0;
      
    memcpy(temp, &buffer[posn], nBytes);
    temp[nBytes / sizeof(wchar_t)] = 0;
      
    // if there is a BOM, skip past it and check to see if we need to swap
    // the byte order around
    if (temp[0] == 0xFEFF || temp[0] == 0xFFFE)
    {
      loc++;
        
      // if we need to swap the byte order
      if (temp[0] != 0xFEFF)
        for (int i = loc; i < (int) wcslen(temp); i++)
          temp[i] = ((temp[i] >> 8) & 0xFF) | (((temp[i]) & 0xFF) << 8);
    }
      
    Set(&temp[loc]);
      
    delete [] temp;
  }
  
  if (__ulFlags & ID3FF_NULL)
    nBytes += sizeof(wchar_t);
    
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
    wchar_t *ourString = (wchar_t *) & buffer[sizeof(wchar_t)];
    
    // we render at sizeof(wchar_t) bytes into the buffer because we make room
    // for the Unicode BOM
    memcpy(&buffer[sizeof(wchar_t)], (uchar *) __sData, 
           nBytes - sizeof(wchar_t));
    
    // now we convert the internal dividers to what they are supposed to be
    for (i = 0; i < nBytes; i++)
      if (ourString[i] == 1)
      {
        wchar_t sub = L'/';
        
        if (__ulFlags & ID3FF_NULLDIVIDE)
          sub = L'\0';
          
        ourString[i] = sub;
      }
  }
  
  if (nBytes)
  {
    // render the BOM
    wchar_t *BOM = (wchar_t *) buffer;
    BOM[0] = 0xFEFF;
  }
  
  if (nBytes == sizeof(wchar_t) && (__ulFlags & ID3FF_NULL))
    for (size_t i = 0; i < sizeof(wchar_t); i++)
      buffer[i] = 0;
    
  __bHasChanged = false;
  
  return nBytes;
}

// $Log: field_string_unicode.cpp,v $
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
