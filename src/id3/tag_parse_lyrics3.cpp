// $Id: tag_parse_lyrics3.cpp,v 1.7 1999/12/01 18:00:59 scott Exp $
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
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "tag.h"
#include "misc_support.h"

luint ID3_CRLFtoLF(char *buffer, luint size)
{
  luint newSize = 0;
  char *dest = buffer;
  char *source = buffer;
  
  if (buffer != NULL && size > 0)
    ID3_THROW(ID3E_NoData);

  while (source < (buffer + size))
  {
    if (*source == 0x0D)
      source++;
    else
      *dest++ = *source++;
  }
    
  newSize = dest - buffer;
    
  return newSize;
}


luint ID3_StripTimeStamps(char *buffer, luint size)
{
  luint newSize = 0;
  char *dest = buffer;
  char *source = buffer;
  
  if (buffer != NULL && size > 0)
    ID3_THROW(ID3E_NoData);

  while (source < (buffer + size))
  {
    if (*source == '[')
      source += 7;
    else
      *dest++ = *source++;
  }
    
  newSize = dest - buffer;
    
  return newSize;
}


void ID3_Tag::ParseLyrics3(void)
{
  if (NULL == __fFileHandle)
    ID3_THROW(ID3E_NoData);

  uchar buffer[18];
    
  fseek(__fFileHandle, -143, SEEK_END);
  fread(buffer, 1, 18, __fFileHandle);
    
  // first check for an ID3v1 tag
  if (memcmp(&buffer[15], "TAG", 3) == 0)
  {
    if (memcmp(&buffer[6], "LYRICSEND", 9) == 0)
    {
      // we have a Lyrics3 v1.00 tag
    }
      
    else if (memcmp(&buffer[6], "LYRICS200", 9) == 0)
    {
      // we have a Lyrics3 v2.00 tag
      luint lyricsSize;
        
      buffer[6] = 0;
      lyricsSize = atoi((char *) buffer);
        
      fseek(__fFileHandle, -18 - lyricsSize, SEEK_CUR);
      fread(buffer, 1, 11, __fFileHandle);
        
      if (memcmp(buffer, "LYRICSBEGIN", 11) == 0)
      {
        luint bytesToRead = lyricsSize - 11;
        uchar *buff2;
          
        __ulExtraBytes += lyricsSize + 9 + 6;
        
        buff2 = new uchar[bytesToRead];
        if (NULL == buff2)
          ID3_THROW(ID3E_NoMemory);

        luint posn = 0;
        bool stampsUsed = false;
            
        fread(buff2, 1, bytesToRead, __fFileHandle);
            
        while (posn < bytesToRead)
        {
          uchar fid[4];
          uchar sizeT[6];
          luint size;
              
          fid[3] = 0;
          sizeT[5] = 0;
              
          memcpy(fid, &buff2[posn], 3);
          memcpy(sizeT, &buff2[posn + 3], 5);
          size = atoi((char *) sizeT);
              
          // the IND field
          if (strcmp((char *) fid, "IND") == 0)
          {
            if (buff2[posn + 8 + 1] == '1')
              stampsUsed = true;
          }
              
          // the TITLE field
          if (strcmp((char *) fid, "ETT") == 0)
          {
            char *text;
            
            text = new char[size + 1];
            if (NULL == text)
              ID3_THROW(ID3E_NoMemory);

            text[size] = 0;
            memcpy(text, &buff2[posn + 8], size);
                  
            ID3_AddTitle(this, text);
                  
            delete[] text;
          }
              
          // the ARTIST field
          if (strcmp((char *) fid, "EAR") == 0)
          {
            char *text;

            text = new char[size + 1];
            if (NULL == text)
              ID3_THROW(ID3E_NoMemory);

            text[size] = 0;
            memcpy(text, &buff2[posn + 8], size);
                  
            ID3_AddArtist(this, text);
                  
            delete[] text;
          }
              
          // the ALBUM field
          if (strcmp((char *) fid, "EAL") == 0)
          {
            char *text;
                
            text = new char[size + 1];
            if (NULL == text)
              ID3_THROW(ID3E_NoMemory);

            text[size] = 0;
            memcpy(text, &buff2[posn + 8], size);
                  
            ID3_AddAlbum(this, text);
                  
            delete[] text;
          }
              
          // the LYRICS field
          if (strcmp((char *) fid, "LYR") == 0)
          {
            char *text;
            luint newSize;
                
            newSize = ID3_CRLFtoLF((char *) & buff2[posn + 8], size);
                
            if (stampsUsed)
              newSize = ID3_StripTimeStamps((char *) & buff2[posn + 8], 
                                            newSize);
                  
            text = new char[newSize + 1];
            if (NULL == text)
              ID3_THROW(ID3E_NoMemory);

            text[newSize] = 0;
                  
            memcpy(text, &buff2[posn + 8], newSize);
                  
            ID3_AddLyrics(this, text);
                  
            delete[] text;
          }
              
          posn += size + 8;
        }
            
        delete[] buff2;
      }
    }
  }
    
  return ;
}

// $Log: tag_parse_lyrics3.cpp,v $
// Revision 1.7  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:21:03  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
