// $Id: field_binary.cpp,v 1.7 1999/11/29 19:26:18 scott Exp $
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

#include <stdio.h>
#include <memory.h>
#include <id3/field.h>


void ID3_Field::Set(const uchar *newData, luint newSize)
{
  Clear();
  
  if (newSize > 0)
  {
    __sData = new uchar[newSize];
    if (NULL == __sData)
      ID3_THROW(ID3E_NoMemory);
      
    memcpy(__sData, newData, newSize);
    __ulSize = newSize;
    
    __eType = ID3FTY_BINARY;
    __bHasChanged = true;
  }
  
  return ;
}


void ID3_Field::Get(uchar *buffer, luint buffLength)
{
  if (NULL == buffer)
    ID3_THROW(ID3E_NoBuffer);
    
  if (__sData != NULL && __ulSize > 0)
  {
    luint actualBytes = MIN(buffLength, __ulSize);
    
    memcpy(buffer, __sData, actualBytes);
  }
  
  return ;
}


void ID3_Field::FromFile(const char *info)
{
  FILE *temp;
  luint fileSize;
  uchar *buffer;
  
  if (NULL == info)
    ID3_THROW(ID3E_NoData);
    
  temp = fopen(info, "rb");
  if (temp != NULL)
  {
    fseek(temp, 0, SEEK_END);
    fileSize = ftell(temp);
    fseek(temp, 0, SEEK_SET);
    
    buffer = new uchar[fileSize];
    if (buffer != NULL)
    {
      fread(buffer, 1, fileSize, temp);
      
      Set(buffer, fileSize);
      
      delete [] buffer;
    }
    
    fclose(temp);
  }
  
  return ;
}


void ID3_Field::ToFile(const char *info)
{
  if (NULL == info)
    ID3_THROW(ID3E_NoData);
    
  if ((__sData != NULL) && (__ulSize > 0))
  {
    FILE *temp;
    
    temp = fopen(info, "wb");
    if (temp != NULL)
    {
      fwrite(__sData, 1, __ulSize, temp);
      fclose(temp);
    }
  }
  
  return ;
}


luint ID3_Field::ParseBinary(const uchar *buffer, const luint posn, const luint buffSize)
{
  luint bytesUsed = 0;
  
  bytesUsed = buffSize - posn;
  
  if (__lFixedLength != -1)
    bytesUsed = MIN(__lFixedLength, bytesUsed);
    
  Set(&buffer[posn], bytesUsed);
  
  __bHasChanged = false;
  
  return bytesUsed;
}


luint ID3_Field::RenderBinary(uchar *buffer)
{
  luint bytesUsed = 0;
  
  bytesUsed = BinSize();
  memcpy(buffer, (uchar *) __sData, bytesUsed);
  
  __bHasChanged = false;
  
  return bytesUsed;
}



// $Log: field_binary.cpp,v $
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/19 17:28:13  scott
// (Set): Updated interface to make parameters const.
// (FromFile): Updated interface to make parameters const.
// (ToFile): Updated interface to make parameters const.
// (ParseBinary): Updated interface to make parameters const.
//
// Revision 1.5  1999/11/15 20:15:50  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
