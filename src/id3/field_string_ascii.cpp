// $Id: field_string_ascii.cpp,v 1.7 1999/11/19 17:34:44 scott Exp $

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

#include <stdlib.h>
#include <id3/field.h>
#include <id3/misc_support.h>


ID3_Field& ID3_Field::operator= (const char *string)
{
  Set(string);
  
  return *this;
}


// the ::Set() function for ASCII

void ID3_Field::Set(const char *sString)
{
  if (sString != NULL)
  {
    Clear();
    size_t nStrLen = (-1 == __lFixedLength) ? strlen(sString) : __lFixedLength;
    wchar_t *temp = new wchar_t[nStrLen + 1];
    if (NULL == temp)
      ID3_THROW(ID3E_NoMemory);

    ID3_ASCIItoUnicode(temp, sString, nStrLen + 1);

    Set(temp);
    delete [] temp;
      
    __eType = ID3FTY_ASCIISTRING;
  }
  
  return ;
}


// the ::Get() function for ASCII

luint ID3_Field::Get(char *buffer, luint maxLength, luint itemNum)
{
  luint bytesUsed = 0;
  wchar_t *temp;
  char *ascii;
  
  temp = new wchar_t[maxLength];
  if (NULL == temp)
    ID3_THROW(ID3E_NoMemory);

  luint len = Get(temp, maxLength, itemNum);

  ascii = new char[len + 1];
  if (NULL == ascii)
    ID3_THROW(ID3E_NoMemory);

  luint length;
        
  ID3_UnicodeToASCII(ascii, temp, len + 1);
        
  length = MIN(strlen(ascii), maxLength);
        
  strncpy(buffer, ascii, length);
  buffer[length] = '\0';
  bytesUsed = length;
        
  delete [] ascii;
    
  delete [] temp;
    
  return bytesUsed;
}


void ID3_Field::Add(const char *sString)
{
  if (sString)
  {
    wchar_t *temp;
    
    temp = new wchar_t[strlen(sString) + 1];
    if (NULL == temp)
    {
      ID3_ASCIItoUnicode(temp, sString, strlen(sString) + 1);
      Add(temp);
      delete [] temp;
      
      __eType = ID3FTY_ASCIISTRING;
    }
  }
  
  return ;
}


luint ID3_Field::ParseASCIIString(const uchar *buffer, const luint posn, const luint buffSize)
{
  luint bytesUsed = 0;
  char *temp = NULL;
  
  if (__lFixedLength != -1)
  {
    bytesUsed = __lFixedLength;
  }
  else
  {
    if (__ulFlags & ID3FF_NULL)
      while ((posn + bytesUsed) < buffSize && buffer[posn + bytesUsed] != '\0')
        bytesUsed++;
    else
      bytesUsed = buffSize - posn;
  }

  if (bytesUsed == 0)
  {
    Set("");
  }
  else
  {
    // Sanity check our indices and sizes before we start copying memory
    if ((bytesUsed > buffSize) || (posn + bytesUsed > buffSize))
    {
      ID3_THROW_DESC(ID3E_BadData, "field information invalid");
    }

    temp = new char[bytesUsed + 1];
    if (NULL == temp)
      ID3_THROW(ID3E_NoMemory);

    memcpy(temp, &buffer[posn], bytesUsed);
    temp[bytesUsed] = '\0';
    Set(temp);
      
    delete [] temp;
  }
  
  if (__ulFlags & ID3FF_NULL)
    bytesUsed++;
    
  __bHasChanged = false;
  
  return bytesUsed;
}


luint ID3_Field::RenderASCIIString(uchar *buffer)
{
  luint bytesUsed = 0;
  char *ascii;
  
  bytesUsed = BinSize();

  if (__sData && __ulSize)
  {
    ascii = new char[__ulSize];
    if (NULL == ascii)
      ID3_THROW(ID3E_NoMemory);

    luint i;
      
    ID3_UnicodeToASCII(ascii, (wchar_t *) __sData, __ulSize);
    memcpy(buffer, (uchar *) ascii, bytesUsed);
      
    // now we convert the internal dividers to what they are supposed to be
    for (i = 0; i < bytesUsed; i++)
      if (buffer[i] == 1)
      {
        char sub = '/';
          
        if (__ulFlags & ID3FF_NULLDIVIDE)
          sub = '\0';
            
        buffer[i] = sub;
      }
        
    if (__ulSize - 1 < bytesUsed)
      for (i = 0; i < (__ulSize - 1 - bytesUsed); i++)
        buffer[bytesUsed + i] = 0x20;
          
    delete [] ascii;
  }
  
  if (bytesUsed == 1 && __ulFlags & ID3FF_NULL)
    buffer[0] = 0;
    
  __bHasChanged = false;
  
  return bytesUsed;
}

// $Log: field_string_ascii.cpp,v $
// Revision 1.7  1999/11/19 17:34:44  scott
// (operator=): Updated interface to make parameters const.
// (Set): Updated interface to make parameters const.  Bug fix for fixed
// length strings.
// (Get): Removed check for nonempty strings so empty strings would be
// set correctly.  Minor code cleanup.
// (Add): Updated interface to make parameters const.  Made variable name
// more descriptive.
//
// Revision 1.6  1999/11/16 22:50:24  scott
// * field_string_ascii.cpp (ParseASCIIString): Added sanity check
// for indices so we don't call memcpy with out-of-bounds indices.
//
// Revision 1.5  1999/11/15 20:16:15  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
