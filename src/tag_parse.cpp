// $Id: tag_parse.cpp,v 1.28 2000/09/30 22:13:38 eldamitri Exp $

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
#include <config.h> // Must include before zlib.h to compile on WinCE
#endif

#include "debug.h"

#include <zlib.h>
#include <string.h>
#include <memory.h>

#include "tag.h"
#include "uint28.h"
#include "utils.h"
#include "reader_decorators.h"
#include "readers.h"
#include "strings.h"

namespace id3
{
  bool parseID3v2(ID3_Tag& tag, ID3_Reader& rdr)
  {
    ID3_Reader::pos_type beg = rdr.getCur();
    tag.Parse(rdr);
    ID3_Reader::pos_type end = rdr.getCur();
    return end != beg;
  }
  bool parseID3v1(ID3_Tag&, ID3_Reader&);
  bool parseLyrics3(ID3_Tag&, ID3_Reader&);
  bool parseLyrics3v2(ID3_Tag&, ID3_Reader&);
  bool parseMusicMatch(ID3_Tag&, ID3_Reader&);
  
  namespace v2
  {
    bool parseFrames(ID3_Tag& tag, ID3_Reader& rdr)
    { 
      ID3_Reader::pos_type beg = rdr.getCur();
      id3::ExitTrigger et(rdr, beg);
      ID3_Reader::pos_type last_pos = beg;
      size_t total_size = 0; 
      size_t frame_size = 0; 
      while (!rdr.atEnd() && rdr.peekChar() != '\0')
      { 
        ID3D_NOTICE( "id3::v2::parseFrames(): rdr.getBeg() = " << rdr.getBeg() );
        ID3D_NOTICE( "id3::v2::parseFrames(): rdr.getCur() = " << rdr.getCur() );
        ID3D_NOTICE( "id3::v2::parseFrames(): rdr.getEnd() = " << rdr.getEnd() );
        last_pos = rdr.getCur();
        ID3_Frame* f = new ID3_Frame; 
        f->SetSpec(tag.GetSpec());
        bool goodParse = f->Parse(rdr);
        frame_size = rdr.getCur() - last_pos;
        ID3D_NOTICE( "id3::v2::parseFrames(): frame_size = " << frame_size );
        total_size += frame_size;
        
        if (frame_size == 0)
        { 
          // There is a problem. 
          // If the frame size is 0, then we can't progress. 
          ID3D_WARNING( "id3::v2::parseFrames(): frame size is 0, can't " <<
                        "continue parsing frames");
          delete f; 
          // Break for now. 
          break; 
        } 
        else if (!goodParse) 
        { 
          // bad parse!  we can't attach this frame.
          ID3D_WARNING( "id3::v2::parseFrames(): bad parse, deleting frame");
          delete f; 
        } 
        else if (f->GetID() != ID3FID_METACOMPRESSION) 
        { 
          ID3D_NOTICE( "id3::v2::parseFrames(): attaching non-compressed " <<
                       "frame");
          // a good, uncompressed frame.  attach away! 
          tag.AttachFrame(f); 
        } 
        else 
        { 
          ID3D_NOTICE( "id3::v2::parseFrames(): parsing ID3v2.2.1 " <<
                       "compressed frame");
          // hmm.  an ID3v2.2.1 compressed frame.  It contains 1 or more
          // compressed frames.  Uncompress and call parseFrames recursively.
          const uchar* bin = f->Field(ID3FN_DATA).GetBinary(); 
          if (*bin != 'z') 
          { 
            // unknown compression method 
            ID3D_WARNING( "id3::v2::parseFrames(): unknown compression id " <<
                          " = '" << *bin << "'" );
          } 
          else 
          { 
            bin++;
            size_t new_size = id3::parseNumber(bin);
            bin += sizeof(uint32);
            size_t old_size = f->GetDataSize() - sizeof(uint32) - 1;
            id3::CompressedMemoryReader cmr(bin, old_size, new_size);
            parseFrames(tag, cmr);
            if (!cmr.atEnd())
            {
              // hmm.  it didn't parse the entire uncompressed data.  wonder
              // why.
              ID3D_WARNING( "id3::v2::parseFrames(): didn't parse entire " <<
                            "id3v2.2.1 compressed memory stream");
            }
          }
          delete f;
        }
        et.setExitPos(rdr.getCur());
      } 
      if (rdr.peekChar() == '\0')
      {
        ID3D_NOTICE( "id3::v2::parseFrames: done parsing, padding at postion " << rdr.getCur() );
      }
      else
      {
        ID3D_NOTICE( "id3::v2::parseFrames: done parsing, [cur, end] = [" << rdr.getCur() << ", " << rdr.getEnd() << "]" );
      }
    } 
  };
};

bool ID3_Tag::Parse(ID3_Reader& reader)
{
  id3::ExitTrigger et(reader);
  ID3_Reader::pos_type beg = reader.getCur();
  
  id3::WindowedReader wr(reader, ID3_TagHeader::SIZE);

  if (!_hdr.Parse(wr) || wr.getCur() == beg)
  {
    return false;
  }

  size_t data_size = _hdr.GetDataSize();
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): data_size = " << data_size);

  wr.setWindow(wr.getCur(), data_size);
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): data window beg = " << wr.getBeg() );
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): data window cur = " << wr.getCur() );
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): data window end = " << wr.getEnd() );
  if (_hdr.GetUnsync())
  {
    id3::UnsyncedReader ur(wr);
    id3::string str;
    str.reserve(ur.getEnd() - ur.getCur());
    while (!ur.atEnd())
    {
      str += (char) ur.readChar();
    }
    id3::MemoryReader mr(str.data(), str.size());
    ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): unsync beg = " << ur.getBeg() );
    ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): unsync cur = " << ur.getCur() );
    ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): unsync end = " << ur.getEnd() );
    id3::v2::parseFrames(*this, mr);
  }
  else
  {
    id3::v2::parseFrames(*this, wr);
  }
  et.setExitPos(wr.getEnd());

  // set the flag which says that the tag hasn't changed
  _changed = false;

  return true;
}

/** Turns a binary tag into a series of ID3_Frame objects attached to the
 ** tag.
 ** 
 ** \code
 **   ID3_Tag myTag;
 **   uchar header[ID3_TAGHEADERSIZE];
 **   uchar *buffer;
 **   luint tagSize;
 ** 
 **   // get ID3_TAGHEADERSIZE from a socket or somewhere
 **   // ...
 ** 
 **   if ((tagSize = ID3_IsTagHeader(ourSourceBuffer)) > -1)
 **   {
 **     // read a further 'tagSize' bytes in
 **     // from our data source
 **     // ...
 **   
 **     if (buffer = new uchar[tagSize])
 **     {
 **       // now we will call ID3_Tag::Parse()
 **       // with these values (explained later)
 **       myTag.Parse(header, buffer);
 **       
 **       // do something with the objects,
 **       // like look for titles, artists, etc.
 **       // ...
 **       
 **       // free the buffer
 **       delete [] buffer;
 **     }
 **   }
 ** \endcode
 ** 
 ** @see ID3_Frame
 ** @param header The byte header read in from the data source.
 ** @param buffer The remainder of the tag (not including the data source) 
 **               read in from the data source.
 **/
size_t ID3_Tag::Parse(const uchar header[ID3_TagHeader::SIZE],
		      const uchar *buffer)
{
  size_t size = ID3_Tag::IsV2Tag(header);
  if (0 == size)
  {
    return 0;
  }
  id3::string buf(reinterpret_cast<const char*>(header), ID3_TagHeader::SIZE);
  buf.append(reinterpret_cast<const char*>(buffer), size);

  id3::MemoryReader mr(buf.data(), buf.size());
  ID3_Reader::pos_type beg = mr.getCur();
  this->Parse(mr);
  return mr.getCur() - beg;

  // set the flag which says that the tag hasn't changed
  _changed = false;
}

void ID3_Tag::ParseFile()
{
  ifstream file;
  if (ID3E_NoError != id3::openReadableFile(this->GetFileName(), file))
  {
    // log this...
    return;
  }
  id3::IFStreamReader ifsr(file);
  id3::WindowedReader wr(ifsr);
  wr.setBeg(wr.getCur());

  _file_tags.clear();
  _file_size = id3::getFileSize(file);

  ID3_Reader::pos_type beg  = wr.getBeg();
  ID3_Reader::pos_type cur  = wr.getCur();
  ID3_Reader::pos_type end  = wr.getEnd();

  ID3_Reader::pos_type last = cur;

  if (_tags_to_parse.test(ID3TT_ID3V2))
  {
    do
    {
      last = cur;
      // Parse tags at the beginning of the file first...
      if (id3::parseID3v2(*this, wr))
      {
        _file_tags.add(ID3TT_ID3V2);
      }
      cur  = wr.getCur();
      wr.setBeg(cur);
    } while (cur > last);
  }

  _prepended_bytes = cur - beg;

  cur = wr.setCur(end);
  do
  {
    last = cur;
    ID3D_NOTICE( "ID3_Tag::ParseFile(): beg = " << wr.getBeg() );
    ID3D_NOTICE( "ID3_Tag::ParseFile(): cur = " << wr.getCur() );
    ID3D_NOTICE( "ID3_Tag::ParseFile(): end = " << wr.getEnd() );
    // ...then the tags at the end
    if (_tags_to_parse.test(ID3TT_MUSICMATCH) && id3::parseMusicMatch(*this, wr))
    {
      _file_tags.add(ID3TT_MUSICMATCH);
      wr.setEnd(wr.getCur());
    }
    if (_tags_to_parse.test(ID3TT_LYRICS3) && id3::parseLyrics3(*this, wr))
    {
      _file_tags.add(ID3TT_LYRICS3);
      wr.setEnd(wr.getCur());
    }
    if (_tags_to_parse.test(ID3TT_LYRICS3V2) && id3::parseLyrics3v2(*this, wr))
    {
      _file_tags.add(ID3TT_ID3V1);
      wr.setEnd(wr.getCur());
    }
    if (_tags_to_parse.test(ID3TT_ID3V1) && id3::parseID3v1(*this, wr))
    {
      wr.setEnd(wr.getCur());
      _file_tags.add(ID3TT_ID3V1);
    }
    cur = wr.getCur();
  } while (cur != last);
  _appended_bytes = end - cur;
}
