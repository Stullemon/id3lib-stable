// $Id: tag_parse_lyrics3.cpp,v 1.13 2000/06/22 20:20:31 eldamitri Exp $

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include "tag.h"
#include "utils.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

bool ID3_IsCRLF(const char* begin, const char* end)
{
  if (begin > end || (end - begin) < 2)
  {
    return false;
  }
  const char* cur = begin;
  return 0x0D == *cur++ && 0x0A == *cur;
}

bool ID3_IsTimeStamp(const char* begin, const char* end)
{
  if (begin > end || (end - begin) < 7)
  {
    return false;
  }
  const char* cur = begin;
  return
    '[' == *cur++ && isdigit(*cur++) && isdigit(*cur++) &&
    ':' == *cur++ && isdigit(*cur++) && isdigit(*cur++) &&
    ']' == *cur;
}

luint ID3_CRLFtoLF(char *buffer, luint size)
{
  luint newSize = 0;
  char *dest = buffer;
  char *source = buffer;
  
  if (NULL == buffer || size == 0)
  {
    // TODO: log this
    return 0;
    // ID3_THROW(ID3E_NoData);
  }

  while (source < (buffer + size))
  {
    if (ID3_IsCRLF(source, buffer + size))
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
  
  if ((buffer == NULL) || (size == 0))
  {
    // TODO: log this
    return 0;
    // ID3_THROW(ID3E_NoData);
  }

  while (source < (buffer + size))
  {
    if (ID3_IsTimeStamp(source, buffer + size))
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

luint ID3_RenderTimeStamp(char* buffer, luint ms, bool lf)
{
  char* dest = buffer;

  // put synch identifier
  *dest++ = '\0';
  
  // put timestamp
  dest += RenderNumber((uchar *)dest, ms, sizeof(uint32));
  if (lf)
  {
    // put the LF
    *dest++ = 0x0A;
  }
  
  return dest - buffer;
}

luint ID3_Lyrics3ToSylt(char *buffer, luint size)
{
  if ((buffer == NULL) || (size == 0))
  {
    // TODO: log this
    return 0;
    //ID3_THROW(ID3E_NoData);
  }

  char* dest = buffer;
  char* source = buffer;
  char* const end = buffer + size;
  
  while (source < end)
  {
    bool lf = false;
    size_t ms = 0;
    size_t count = 0;
    while (ID3_IsTimeStamp(source, end))
    {
      if (count++ > 0)
      {
        source += 7;
      }
      else
      {
        // timestamp found skip [
        source++;
        
        // get minutes and ms
        size_t minutes = strtol(source, NULL, 10);
        
        // skip :
        source += 3;
        
        size_t seconds = strtol(source, NULL, 10);
        
        // skip ]
        source += 3;
        
        // get seconds and ms
        ms = ((60 * minutes) + seconds) * 1000;
      }
    }
    while (source < end && !ID3_IsTimeStamp(source, end))
    {
      if (0x0A == *source && 
          (source + 1 >= end || ID3_IsTimeStamp(source + 1, end)))
      {
        lf = true;
        source++;
      }
      else
      {
        *dest++ = *source++;
      }
    }
    dest += ID3_RenderTimeStamp(dest, ms, lf);
  }
  
  return dest - buffer;
}

void ID3_Tag::ParseLyrics3()
{
  if (NULL == __file_handle)
  {
    // TODO: log this
    return;
    // ID3_THROW(ID3E_NoData);
  }
  
  char id_buffer[6 + 9 + 3];
  
  fseek(__file_handle, -(6 + 9 + 128), SEEK_END);
  fread(id_buffer, 1, (6 + 9 + 3), __file_handle);

  // first check for an ID3v1 tag
  if (memcmp(&id_buffer[6 + 9], "TAG", 3) == 0)
  {
    // check for lyrics
    if (memcmp(&id_buffer[6], "LYRICSEND", 9) == 0)
    {
      // we have a Lyrics3 v1.00 tag

      if (__file_size < 128 + 9 + 11)
      {
        // the file size isn't large enough to actually hold lyrics
        return;
      }

      // reserve enough space for lyrics3 + id3v1 tag
      const size_t max_lyr_size = 11 + 5100 + 9 + 128;

      size_t lyr_buffer_size = MIN(max_lyr_size, __file_size);
      fseek(__file_handle, -(lyr_buffer_size), SEEK_END);

      char lyr_buffer[max_lyr_size];
      fread(lyr_buffer, 1, lyr_buffer_size, __file_handle);

      // search for LYRICSBEGIN
      char* lyr_begin = strstr(lyr_buffer, "LYRICSBEGIN");
      char* lyr_end = &lyr_buffer[lyr_buffer_size - 128 - 9];

      if (lyr_begin && lyr_begin < lyr_end)
      {
        // skip past the beginning keyword
        lyr_begin += 11;
      }
      else
      {
        // not a lyrics3 tag
        return;
      }

      size_t lyr_size = ID3_CRLFtoLF(lyr_begin, MAX(0, lyr_end - lyr_begin));

      lyr_begin[lyr_size] = '\0';

      ID3_Frame *frame = ID3_AddLyrics(this, lyr_begin);
      if (frame)
      {
        frame->Field(ID3FN_LANGUAGE) = "eng";
        frame->Field(ID3FN_DESCRIPTION) = "Converted from Lyrics3 v1.00";
      }

      // if we got to here we must have some lyrics
      __file_tags.add(ID3TT_LYRICS);
    }
  
    else if (memcmp(&id_buffer[6], "LYRICS200", 9) == 0)
    {
      // we have a Lyrics3 v2.00 tag
      __file_tags.add(ID3TT_LYRICS);
      
      ID3_Frame *lyr_frame = NULL;
      char *textInf = NULL;
      
      id_buffer[6] = '\0';
      size_t lyr_size = atoi(id_buffer);
      
      fseek(__file_handle, -(lyr_size + 6 + 9 + 3), SEEK_CUR);
      fread(id_buffer, 1, 11, __file_handle);

      if (memcmp(id_buffer, "LYRICSBEGIN", 11) != 0)
      {
        // not a lyrics v2.00 tag
        return;
      }

      __ending_bytes += lyr_size + 9 + 6;
      lyr_size -= 11;
      char* lyr_buffer = new char[lyr_size];

      luint posn = 0;
      bool has_time_stamps = false;

      fread(lyr_buffer, 1, lyr_size, __file_handle);

      while (posn + 3 + 5 <= lyr_size)
      {
        char fld_size_str[6];
        char* fld_id = &lyr_buffer[posn];

        fld_size_str[5] = '\0';
        memcpy(fld_size_str, &lyr_buffer[posn + 3], 5);
        size_t fld_size = atoi(fld_size_str);

        if (posn + 3 + 5 + fld_size > lyr_size)
        {
          // new field doesn't fit in the tag.  bail.
          return;
        }

        posn += 3 + 5;
        char* fld_data = &lyr_buffer[posn];

        // the IND field
        if (memcmp(fld_id, "IND", 3) == 0)
        {
          has_time_stamps = (*(fld_data + 1) == '1');
        }

        // the TITLE field
        else if (memcmp(fld_id, "ETT", 3) == 0)
        {
          char *text = new char[fld_size + 1];

          text[fld_size] = '\0';
          memcpy(text, fld_data, fld_size);

          ID3_AddTitle(this, text);

          delete [] text;
        }

        // the ARTIST field
        else if (memcmp(fld_id, "EAR", 3) == 0)
        {
          char *text = new char[fld_size + 1];
          text[fld_size] = '\0';

          memcpy(text, fld_data, fld_size);

          ID3_AddArtist(this, text);

          delete[] text;
        }

        // the ALBUM field
        else if (memcmp(fld_id, "EAL", 3) == 0)
        {
          char *text = new char[fld_size + 1];
          text[fld_size] = '\0';
          memcpy(text, fld_data, fld_size);

          ID3_Frame* frame = ID3_AddAlbum(this, text, true);

          delete[] text;
        }

        // the Lyrics/Music AUTHOR field
        else if (memcmp(fld_id, "AUT", 3) == 0)
        {
          char *text = new char[fld_size + 1];

          text[fld_size] = '\0';
          memcpy(text, fld_data, fld_size);

          ID3_AddLyricist(this, text);

          delete[] text;
        }

        // the INFORMATION field
        else if (memcmp(fld_id, "INF", 3) == 0)
        {
          size_t adj_size = ID3_CRLFtoLF(fld_data, fld_size);

          textInf = new char[adj_size + 1];
          textInf[adj_size] = '\0';

          memcpy(textInf, fld_data, adj_size);

          // if already found the lyrics text use this field as description
          if (NULL != lyr_frame)
          {
            lyr_frame->Field(ID3FN_DESCRIPTION) = textInf;
          }
        }

        // the LYRICS field
        else if (memcmp(fld_id, "LYR", 3) == 0)
        {
          size_t adj_size = ID3_CRLFtoLF(fld_data, fld_size);

          // if already found an INF field, use it as description
          const char* description = 
            (textInf ? textInf : "Converted from Lyrics3 v2.00");
          if (!has_time_stamps)
          {
            char* text = new char[adj_size + 1];
            text[adj_size] = '\0';
            memcpy(text, fld_data, adj_size);

            lyr_frame = ID3_AddLyrics(this, text);

            delete[] text;

            if (lyr_frame)
            {
              lyr_frame->Field(ID3FN_LANGUAGE) = "eng";
              lyr_frame->Field(ID3FN_DESCRIPTION) = description;
            }

          }
          else
          {
            // convert lyrics into a SYLT frame Content Descriptor
            adj_size = ID3_Lyrics3ToSylt (fld_data, adj_size);
            
            char* text = new char[adj_size + 1];
            text[adj_size] = '\0';
            memcpy(text, fld_data, adj_size);
            
            lyr_frame =
              ID3_AddSyncLyrics(this, "eng", description, (uchar *)text, adj_size);

            delete[] text;

            if (lyr_frame)
            {
              lyr_frame->Field(ID3FN_TIMESTAMPFORMAT) = ID3TSF_MS;
              lyr_frame->Field(ID3FN_CONTENTTYPE) = ID3CT_LYRICS;
            }

          }
        }

        posn += fld_size;
      }

      delete [] lyr_buffer;
      if (NULL != textInf)
      {
        delete[] textInf;
      }
    }
  }
}
