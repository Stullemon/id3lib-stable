// $Id: tag_parse_lyrics3.cpp,v 1.10 2000/01/04 15:42:49 eldamitri Exp $
// 
// This program is free software; you can distribute it and/or modify it under
// the terms discussed in the COPYING file, which should have been included
// with this distribution.
//  
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the COPYING file for more details.
//  
// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where
// to send such submissions.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory.h>
#include "tag.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

luint ID3_CRLFtoLF(char *buffer, luint size)
{
  luint newSize = 0;
  char *dest = buffer;
  char *source = buffer;
  
  if (NULL == buffer || size == 0)
  {
    ID3_THROW(ID3E_NoData);
  }

  while (source < (buffer + size))
  {
    if (*source == 0x0D)
    {
      source++;
    }
    else
    {
      *dest++ = *source++;
    }
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
  {
    ID3_THROW(ID3E_NoData);
  }

  while (source < (buffer + size))
  {
    if (*source == '[')
    {
      source += 7;
    }
    else
    {
      *dest++ = *source++;
    }
  }
    
  newSize = dest - buffer;
    
  return newSize;
}


void ID3_Tag::ParseLyrics3(void)
{
  if (NULL == __fFileHandle)
  {
    ID3_THROW(ID3E_NoData);
  }

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
        {
          ID3_THROW(ID3E_NoMemory);
        }

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
            {
              stampsUsed = true;
            }
          }
              
          // the TITLE field
          if (strcmp((char *) fid, "ETT") == 0)
          {
            char *text;
            
            text = new char[size + 1];
            if (NULL == text)
            {
              ID3_THROW(ID3E_NoMemory);
            }

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
            {
              ID3_THROW(ID3E_NoMemory);
            }

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
            {
              ID3_THROW(ID3E_NoMemory);
            }

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
            {
              newSize = ID3_StripTimeStamps((char *) & buff2[posn + 8], 
                                            newSize);
            }
                  
            text = new char[newSize + 1];
            if (NULL == text)
            {
              ID3_THROW(ID3E_NoMemory);
            }

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
// Revision 1.10  2000/01/04 15:42:49  eldamitri
// * include/id3/field.h:
// * include/id3/int28.h:
// * include/id3/misc_support.h:
// * include/id3/tag.h:
// * include/id3/types.h:
// * src/id3/dll_wrapper.cpp
// * src/id3/error.cpp
// * src/id3/field.cpp
// * src/id3/field_binary.cpp
// * src/id3/field_integer.cpp
// * src/id3/field_string_ascii.cpp
// * src/id3/field_string_unicode.cpp
// * src/id3/frame.cpp
// * src/id3/frame_parse.cpp
// * src/id3/frame_render.cpp
// * src/id3/header.cpp
// * src/id3/header_frame.cpp
// * src/id3/header_tag.cpp
// * src/id3/int28.cpp
// * src/id3/misc_support.cpp
// * src/id3/tag.cpp
// * src/id3/tag_file.cpp:
// * src/id3/tag_find.cpp:
// * src/id3/tag_parse.cpp:
// * src/id3/tag_parse_lyrics3.cpp:
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.9  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.8  1999/12/16 14:40:02  scott
// (ID3_CRLFtoLF): Fixed memory allocation bug (thanks Alexander Voronin).
//
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
