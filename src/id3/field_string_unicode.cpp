// $Id: field_string_unicode.cpp,v 1.5 1999/11/15 20:16:33 scott Exp $

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

ID3_Field& ID3_Field::operator= (wchar_t *string)
{
  Set(string);
  
  return *this;
}


// this is Set()

void ID3_Field::Set(wchar_t *string)
{
  luint bytesUsed = wcslen(string);
  
  // we can simply increment the bytesUsed count here because we just pilfer
  // the NULL which is present in the string which was passed to us
  if (__ulFlags & ID3FF_NULL)
    bytesUsed++;
    
  // doubling the bytesUsed because Unicode is twice the size of ASCII
  bytesUsed *= sizeof(wchar_t);
  
  Set((uchar *) string, bytesUsed);
  
  __eType = ID3FTY_UNICODESTRING;
  __bHasChanged = true;
  
  return ;
}


void ID3_Field::Add(wchar_t *string)
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

luint ID3_Field::Get(wchar_t *buffer, luint maxChars, luint itemNum)
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
        
      // we subtract 1 here so we have room for the NULL terminator
      maxChars--;
      
      if (NULL == buffer)
        ID3_THROW(ID3E_NoBuffer);

      luint actualChars = MIN(maxChars, sourceLen);
        
      wcsncpy(buffer, source, actualChars);
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


luint ID3_Field::ParseUnicodeString(uchar *buffer, luint posn, luint buffSize)
{
  luint bytesUsed = 0;
  wchar_t *temp = NULL;
  
  if (__lFixedLength != -1)
    bytesUsed = __lFixedLength;
  else
  {
    if (__ulFlags & ID3FF_NULL)
      while ((posn + bytesUsed) < buffSize &&
             !(buffer[posn + bytesUsed] == 0 && 
               buffer[posn + bytesUsed + 1] == 0))
        bytesUsed += 2;
    else
      bytesUsed = buffSize - posn;
  }
  
  if (bytesUsed > 0)
  {
    temp = new wchar_t[(bytesUsed / sizeof(wchar_t)) + 1];
    if (NULL == temp)
      ID3_THROW(ID3E_NoMemory);

    luint loc = 0;
      
    memcpy(temp, &buffer[posn], bytesUsed);
    temp[bytesUsed / sizeof(wchar_t)] = 0;
      
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
    bytesUsed += 2;
    
  __bHasChanged = false;
  
  return bytesUsed;
}


luint ID3_Field::RenderUnicodeString(uchar *buffer)
{
  luint bytesUsed = 0;
  
  bytesUsed = BinSize();
  
  if (NULL != __sData && __ulSize && bytesUsed)
  {
    luint i;
    wchar_t *ourString = (wchar_t *) & buffer[sizeof(wchar_t)];
    
    // we render at sizeof(wchar_t) bytes into the buffer because we make room
    // for the Unicode BOM
    memcpy(&buffer[sizeof(wchar_t)], (uchar *) __sData, 
           bytesUsed - sizeof(wchar_t));
    
    // now we convert the internal dividers to what they are supposed to be
    for (i = 0; i < bytesUsed; i++)
      if (ourString[i] == 1)
      {
        wchar_t sub = L'/';
        
        if (__ulFlags & ID3FF_NULLDIVIDE)
          sub = L'\0';
          
        ourString[i] = sub;
      }
  }
  
  if (bytesUsed)
  {
    // render the BOM
    wchar_t *BOM = (wchar_t *) buffer;
    BOM[0] = 0xFEFF;
  }
  
  if (bytesUsed == 2 && (__ulFlags & ID3FF_NULL))
    buffer[0] = buffer[1] = 0;
    
  __bHasChanged = false;
  
  return bytesUsed;
}

// $Log: field_string_unicode.cpp,v $
// Revision 1.5  1999/11/15 20:16:33  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
