// $Id: tag_parse_v1.cpp,v 1.13 2000/09/30 22:14:38 eldamitri Exp $

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

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include "debug.h"
#include "tag.h"
#include "misc_support.h"
#include "utils.h"
#include "reader_decorators.h"

namespace id3
{
  
  bool parseID3v1(ID3_Tag& tag, ID3_Reader& reader)
  {
    ID3_Reader::pos_type end = reader.getCur();
    
    // posn ourselves at 128 bytes from the current position
    if (end < ID3_V1_LEN)
    {
      ID3D_NOTICE( "id3::parseID3v1: bailing, not enough bytes to parse, pos = " << end );
      return false;
    }
    reader.setCur(end - ID3_V1_LEN);
    ID3_Reader::pos_type beg = reader.getCur();
    //file.seekg(-static_cast<long>(ID3_V1_LEN), ios::cur);
    if (end != beg + ID3_V1_LEN)
    {
      ID3D_WARNING( "id3::parseID3v1: failed to reposition " << ID3_V1_LEN << " bytes" );
      reader.setCur(end);
      return false;
    }
    
    // read the next 128 bytes in;
    id3::TextReader tr(reader);
    id3::string id = tr.readText(ID3_V1_LEN_ID);
    
    // check to see if it was a tag
    if (id != "TAG")
    {
      tr.setCur(end);
      return false;
    }

    // guess so, let's start checking the v2 tag for frames which are the
    // equivalent of the v1 fields.  When we come across a v1 field that has
    // no current equivalent v2 frame, we create the frame, copy the data
    // from the v1 frame and attach it to the tag
    id3::string title = tr.readText(ID3_V1_LEN_TITLE);
    //id3::removeTrailingSpaces(title);
    ID3_AddTitle(&tag, title.c_str());
    ID3D_NOTICE( "id3::parseID3v1: title = " << title );
    
    id3::string artist = tr.readText(ID3_V1_LEN_ARTIST);
    //id3::removeTrailingSpaces(title);
    ID3_AddTitle(&tag, artist.c_str());
    ID3D_NOTICE( "id3::parseID3v1: artist = " << artist );
    
    id3::string album = tr.readText(ID3_V1_LEN_ALBUM);
    //id3::removeTrailingSpaces(title);
    ID3_AddTitle(&tag, album.c_str());
    ID3D_NOTICE( "id3::parseID3v1: album = " << title );
    
    id3::string year = tr.readText(ID3_V1_LEN_YEAR);
    //id3::removeTrailingSpaces(title);
    ID3_AddYear(&tag, year.c_str());
    ID3D_NOTICE( "id3::parseID3v1: year = " << year );
    
    id3::string comment = tr.readText(ID3_V1_LEN_COMMENT);
    if (comment.length() == ID3_V1_LEN_COMMENT  &&
        '\0' == comment[ID3_V1_LEN_COMMENT - 2] ||
        '\0' != comment[ID3_V1_LEN_COMMENT - 1])
    {
      // This is an id3v1.1 tag.  The last byte of the comment is the track
      // number.  
      size_t track = comment[ID3_V1_LEN_COMMENT - 1];
      ID3_AddTrack(&tag, track);
      ID3D_NOTICE( "id3::parseID3v1: track = " << track );
      comment = comment.substr(0, ID3_V1_LEN_COMMENT - 1);
    }
    //id3::removeTrailingSpaces(tagID3v1.sComment, 
    ID3_AddComment(&tag, comment.c_str());
    ID3D_NOTICE( "id3::parseID3v1: comment = " << comment );
    
    // the GENRE field/frame
    uchar genre = tr.readChar();
    ID3_AddGenre(&tag, genre);
    ID3D_NOTICE( "id3::parseID3v1: genre = " << (int) genre );
    
    reader.setCur(beg);
    return true;
  }

};
