// $Id: tag_parse_musicmatch.cpp,v 1.8 2000/09/11 07:46:32 eldamitri Exp $

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

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "tag.h"
#include "utils.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

size_t MM_ParseNum(const char* arr, size_t numBytes)
{
  size_t val = 0;
  for (size_t i = 0; i < numBytes; i++)
  {
    val += (static_cast<size_t>(0xFF & arr[i]) << (i * 8));
  }
  return val;
}

ID3_Frame* MM_ParseTextField(fstream& file, ID3_FrameID id, const char* desc = "")
{
  char size_bytes[2];
  size_t size = 0;
  file.read(size_bytes, 2);
  size = MM_ParseNum(size_bytes, 2);
  ID3_Frame* frame = NULL;
  if (size > 0)
  {
    char* text = new char[size + 1];
    text[size] = '\0';
    file.read(text, size);
    if (ID3FID_SONGLEN == id)
    {
      size_t len = ID3_TimeToSeconds(text, size) * 1000;
      size_t digits = 1;
      for (size_t max = 10; max < len; digits++, max *= 10)
      {
        continue;
      }
      char* len_str = new char[digits + 1];
      sprintf(len_str, "%lu", static_cast<long unsigned>(len));
      delete [] text;
      text = len_str;
    }
    else
    {
      size = ID3_CRLFtoLF(text, size);
      text[size] = '\0';
    }
    frame = new ID3_Frame(id);
    if (frame)
    {
      if (frame->Contains(ID3FN_TEXT))
      {
        frame->Field(ID3FN_TEXT).Set(text);
      }
      else if (frame->Contains(ID3FN_URL))
      {
        frame->Field(ID3FN_URL).Set(text);
      }
      if (frame->Contains(ID3FN_LANGUAGE))
      {
        frame->Field(ID3FN_LANGUAGE).Set("XXX");
      }
      if (frame->Contains(ID3FN_DESCRIPTION))
      {
        frame->Field(ID3FN_DESCRIPTION).Set(desc);
      }
    }
    delete [] text;
  }
  return frame;
}

size_t ParseMusicMatch(ID3_Tag& tag, fstream& file)
{
  char sig[8];
  size_t tag_bytes = 0;
  if (!file.is_open())
  {
    // TODO: log this
    return tag_bytes;
    // ID3_THROW(ID3E_NoData);
  }
  
  char id_buffer[48];
  
  if (file.tellg() < 48)
  {
    return 0;
  }
  file.seekg(-48, ios::cur);
  file.read(id_buffer, 48);

  if (memcmp(id_buffer, "Brava Software Inc.             ", 32) != 0)
  {
    // not a music match tag
    return 0;
  }
  char sVersion[4+1];
  sVersion[4] = '\0';
  strncpy(sVersion, &id_buffer[32], 4);
  size_t nVersion = static_cast<size_t>(atof(sVersion) * 100.);
    
  streampos tag_end = file.tellg();
  streampos tag_beg = tag_end;

  if (file.tellg() < 68)
  {
    return 0;
  }
  file.seekg(-68, ios::cur);
  streampos sig_pos = file.tellg();
    
  char sOffsetBuffer[20];
  file.read(sOffsetBuffer, 20);

  size_t metadata_size = 0;

  if (nVersion <= 300)
  {
    // All MusicMatch tags up to and including version 3.0 had metadata 
    // sections exactly 7868 bytes in length.
    metadata_size = 7868;
  }
  else
  {
    // MusicMatch tags after version 3.0 had three possible lengths for their
    // metadata sections.  We can determine which it was by searching for
    // the version section signature that should precede the metadata section
    // by exactly 256 bytes.
    size_t possible_sizes[] = { 8132, 8004, 7936 };

    for (size_t i = 0; i < sizeof(possible_sizes)/sizeof(size_t); ++i)
    {
      file.seekg(sig_pos);
        
      // Our offset will be exactly 256 bytes prior to our potential metadata
      // section
      int offset = possible_sizes[i] + 256;
      if (file.tellg() < offset)
      {
        // if our filesize is less than the offset, then it can't possibly
        // be the correct offset, so try again.
        continue;
      }
      file.seekg(-offset, ios::cur);

      // now read in the signature to see if it's a match
      file.read(sig, 8);
        
      if (memcmp(sig, "18273645", 8) == 0)
      {
        metadata_size = possible_sizes[i];
        break;
      }
    }
  }
  if (0 == metadata_size)
  {
    // if we didn't establish a size for the metadata, then something is
    // wrong.  probably should log this.
    return 0;
  }

  // parse the offset pointers to determine the actual sizes of all the 
  // sections
  size_t section_sizes[5];
  size_t tag_size = metadata_size + 20 + 48;

  // we already know the size of the last section
  section_sizes[4] = metadata_size;
    
  size_t last_offset = 0;
  for (int i = 0; i < 5; i++)
  {
    size_t this_offset = MM_ParseNum(&sOffsetBuffer[i*4], 4);
    //ASSERT(this_offset > last_offset);
    if (i > 0)
    {
      size_t section_size = this_offset - last_offset;
      section_sizes[i-1] = section_size;
      tag_size += section_size;
    }
    last_offset = this_offset;
  }

  // now check to see that our tag size is reasonable
  if (static_cast<size_t>(tag_end) < tag_size)
  {
    // Ack!  The tag size doesn't jive with the tag's ending position in
    // the file.  Bail!
    return 0;
  }

  file.seekg(tag_end - tag_size);
  tag_beg = file.tellg();

  // Now calculate the actual offsets
  streampos offsets[5];
  offsets[0] = tag_beg;
  for (size_t i = 0; i < 4; ++i)
  {
    offsets[i+1] = offsets[i] + section_sizes[i];
  }

  // now check for a tag header and adjust the tag_beg pointer appropriately
  if (tag_beg >= 256)
  {
    file.seekg(tag_beg - 256);
    file.read(sig, 8);
    if (memcmp(sig, "18273645", 8) == 0)
    {
      file.seekg(tag_beg - 256);
      tag_beg = file.tellg();
      file.ignore(256);
    }
    else
    {
      // no header.  do nothing.
    }
  }
    
  // Now parse the various sections...

  // Parse the image extension at offset 0
  file.seekg(offsets[0]);
  char sImgExt[4+1];
  sImgExt[4] = '\0';
  file.read(sImgExt, 4);
  RemoveTrailingSpaces(sImgExt, 4);

  // Parse the image binary at offset 1
  file.seekg(offsets[1]);
  char sImgSize[4];
  file.read(sImgSize, 4);
  size_t nImgSize = MM_ParseNum(sImgSize, 4);
  if (0 == nImgSize)
  {
    // no image binary.  don't do anything.
  }
  else if (offsets[0] + 4 + nImgSize > static_cast<size_t>(offsets[2]))
  {
    // Ack!  The image size given extends beyond the next offset!  This is 
    // not good...  log?
  }
  else
  {
    uchar* img_data = new uchar[nImgSize];
    file.read(img_data, nImgSize);
    ID3_Frame* frame = new ID3_Frame(ID3FID_PICTURE);
    if (frame)
    {
      char mimetype[11];
      strcpy(mimetype, "image/");
      strcat(mimetype, sImgExt);
      frame->Field(ID3FN_MIMETYPE).Set(mimetype);
      frame->Field(ID3FN_IMAGEFORMAT).Set("");
      frame->Field(ID3FN_PICTURETYPE).Set(static_cast<unsigned int>(0));
      frame->Field(ID3FN_DESCRIPTION).Set("");
      frame->Field(ID3FN_DATA).Set(img_data, nImgSize);
      tag.AttachFrame(frame);
    }
  }

  //file.seekg(offsets[2]);
  //file.seekg(offsets[3]);
  
  file.seekg(offsets[4]);
          
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_TITLE));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_ALBUM));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_LEADARTIST));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_CONTENTTYPE));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Tempo"));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Mood"));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Situation"));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Preference"));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_SONGLEN));
  
  // The next 12 bytes can be ignored.  The first 8 represent the 
  // creation date as a 64 bit floating point number.  The last 4 are
  // for a play counter.
  file.ignore(12);
  
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Path"));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Serial"));
  
  // 2 bytes for track
  char trk_bytes[2];
  file.read(trk_bytes, 2);
  size_t trk_num = MM_ParseNum(trk_bytes, 2);
  if (trk_num > 0)
  {
    char trk_str[5];
    sprintf(trk_str, "%lu", static_cast<unsigned long>(trk_num));
    ID3_Frame* frame = new ID3_Frame(ID3FID_TRACKNUM);
    if (frame)
    {
      frame->Field(ID3FN_TEXT).Set(trk_str);
      tag.AttachFrame(frame);
    }
  }
  
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Notes"));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_Bio"));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_UNSYNCEDLYRICS));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_WWWARTIST));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_WWWCOMMERCIALINFO));
  tag.AttachFrame(MM_ParseTextField(file, ID3FID_COMMENT, 
                                    "MusicMatch_ArtistEmail"));
  
  // email?
  
  return tag_size;
}
