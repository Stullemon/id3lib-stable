// $Id: tag_parse_v1.cpp,v 1.13 2000/01/04 15:42:49 eldamitri Exp $
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
#include <cstring>
#include <memory.h>
#include "tag.h"
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

void ID3_RemoveTrailingSpaces(char *buffer, luint length)
{
  for (lsint i = length - 1; i > -1 && 0x20 == buffer[i]; i--)
  {
    buffer[i] = '\0';
  }
      
  return ;
}


void ID3_Tag::ParseID3v1(void)
{
  if (NULL == __fFileHandle)
  {
    ID3_THROW(ID3E_NoData);
  }

  ID3V1_Tag tagID3v1;
    
  // posn ourselves at 128 bytes from the end of the file
  if (fseek(__fFileHandle, 0-LEN_V1, SEEK_END) != 0)
  {
    // TODO:  This is a bad error message.  Make it more descriptive
    ID3_THROW(ID3E_NoData);
  }
    
    
  // read the next 128 bytes in;
  if (fread(tagID3v1.sID, 1, LEN_V1_ID, __fFileHandle) != LEN_V1_ID)
  {
    // TODO:  This is a bad error message.  Make it more descriptive
    ID3_THROW(ID3E_NoData);
  }
    
  // check to see if it was a tag
  if (memcmp(tagID3v1.sID, "TAG", LEN_V1_ID) == 0)
  {
    // guess so, let's start checking the v2 tag for frames which are the
    // equivalent of the v1 fields.  When we come across a v1 field that has
    // no current equivalent v2 frame, we create the frame, copy the data
    // from the v1 frame and attach it to the tag
      
    __bHasV1Tag = true;
    __ulExtraBytes += LEN_V1;

    // the TITLE field/frame
    if (fread(tagID3v1.sTitle, 1, LEN_V1_TITLE, __fFileHandle) != LEN_V1_TITLE)
    {
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
    }
    tagID3v1.sTitle[LEN_V1_TITLE] = '\0';
    ID3_RemoveTrailingSpaces(tagID3v1.sTitle,  LEN_V1_TITLE);
    ID3_AddTitle(this, tagID3v1.sTitle);
    
    // the ARTIST field/frame
    if (fread(tagID3v1.sArtist, 1, LEN_V1_ARTIST, __fFileHandle) != 
        LEN_V1_ARTIST)
    {
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
    }
    tagID3v1.sArtist[LEN_V1_ARTIST] = '\0';
    ID3_RemoveTrailingSpaces(tagID3v1.sArtist, LEN_V1_ARTIST);
    ID3_AddArtist(this, tagID3v1.sArtist);
  
    // the ALBUM field/frame
    if (fread(tagID3v1.sAlbum, 1, LEN_V1_ALBUM, __fFileHandle) != LEN_V1_ALBUM)
    {
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
    }
    tagID3v1.sAlbum[LEN_V1_ALBUM] = '\0';
    ID3_RemoveTrailingSpaces(tagID3v1.sAlbum,  LEN_V1_ALBUM);
    ID3_AddAlbum(this, tagID3v1.sAlbum);
  
    // the YEAR field/frame
    if (fread(tagID3v1.sYear, 1, LEN_V1_YEAR, __fFileHandle) != LEN_V1_YEAR)
    {
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
    }
    tagID3v1.sYear[LEN_V1_YEAR] = '\0';
    ID3_RemoveTrailingSpaces(tagID3v1.sYear,   LEN_V1_YEAR);
    ID3_AddYear(this, tagID3v1.sYear);
  
    // the COMMENT field/frame
    if (fread(tagID3v1.sComment, 1, LEN_V1_COMMENT, __fFileHandle) !=
        LEN_V1_COMMENT)
    {
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
    }
    tagID3v1.sComment[LEN_V1_COMMENT] = '\0';
    if ('\0' != tagID3v1.sComment[LEN_V1_COMMENT - 2] ||
        '\0' == tagID3v1.sComment[LEN_V1_COMMENT - 1])
    {
      ID3_RemoveTrailingSpaces(tagID3v1.sComment, LEN_V1_COMMENT);
    }
    else
    {
      // This is an id3v1.1 tag.  The last byte of the comment is the track
      // number.  
      ID3_RemoveTrailingSpaces(tagID3v1.sComment, LEN_V1_COMMENT - 1);
      ID3_AddTrack(this, tagID3v1.sComment[LEN_V1_COMMENT - 1]);
    }
    ID3_AddComment(this, tagID3v1.sComment, STR_V1_COMMENT_DESC);
      
    // the GENRE field/frame
    fread(&tagID3v1.ucGenre, 1, LEN_V1_GENRE, __fFileHandle);
    ID3_AddGenre(this, tagID3v1.ucGenre);
  }
    
  return ;
}

// $Log: tag_parse_v1.cpp,v $
// Revision 1.13  2000/01/04 15:42:49  eldamitri
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
// Revision 1.12  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.11  1999/12/16 14:40:31  scott
// Fixed inclusion config.h (thanks Alexander Voronin).
//
// Revision 1.10  1999/12/05 05:34:27  scott
// (ParseID3v1): Added STR_V1_COMMENT_DESC as description parameter to
// call to ID3_AddComment
//
// Revision 1.9  1999/12/01 22:19:51  scott
// (ParseID3v1): Minor fix for windows compatibility (thanks elrod).
//
// Revision 1.8  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/19 19:12:46  scott
// (ID3_RemoveTrailingSpaces): Minor code cleanup.
// (ParseID3v1): Massive recoding of method to make use of new functions
// defined in misc_support.  Simplifies flow of method.
//
// Revision 1.5  1999/11/15 20:21:15  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
