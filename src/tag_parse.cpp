// $Id: tag_parse.cpp,v 1.27 2000/09/27 08:36:25 eldamitri Exp $

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

size_t ID3_ParseFrames(ID3_Tag& tag, ID3_Reader& rdr)
{ 
  ID3_Reader::pos_type beg = rdr.getCur();
  ID3_Reader::pos_type last_pos = beg;
  size_t total_size = 0; 
  size_t frame_size = 0; 
  id3::WindowedReader wr(rdr);
  while (wr.getCur() < wr.getEnd() && wr.peekChar() != '\0')
  { 
    ID3D_NOTICE( "ID3_ParseFrames(): wr.getBeg() = " << wr.getBeg() );
    ID3D_NOTICE( "ID3_ParseFrames(): wr.getCur() = " << wr.getCur() );
    ID3D_NOTICE( "ID3_ParseFrames(): wr.getEnd() = " << wr.getEnd() );
    last_pos = wr.getCur();
    ID3_Frame* f = new ID3_Frame; 
    f->SetSpec(tag.GetSpec());
    f->Parse(wr);
    frame_size = wr.getCur() - last_pos;
    ID3D_NOTICE( "ID3_ParseFrames(): frame_size = " << frame_size );
    total_size += frame_size;
     
    if (f->BadParse()) 
    { 
      ID3D_WARNING( "ID3_ParseFrames(): bad parse, deleting frame");
      // bad parse!  we can't attach this frame.  should probably log this 
      // TODO: log this 
      delete f; 
    } 
    else if (frame_size == 0)
    { 
      // There is a problem. 
      // If the frame size is 0, then we can't progress. 
      ID3D_WARNING( "ID3_ParseFrames(): frame size is 0, can't continue parsing frames");
      delete f; 
      // Break for now. 
      break; 
    } 
    else if (f->GetID() != ID3FID_METACOMPRESSION) 
    { 
      ID3D_NOTICE( "ID3_ParseFrames(): attaching non-compressed frame");
      // a good, uncompressed frame.  attach away! 
      tag.AttachFrame(f); 
    } 
    else 
    { 
      ID3D_NOTICE( "ID3_ParseFrames(): parsing ID3v2.2.1 compressed frame");
      // hmm.  an ID3v2.2.1 compressed frame.  It contains 1 or more compressed
      // frames.  Uncompress and call ParseFrames recursively.
      const uchar* bin = f->Field(ID3FN_DATA).GetBinary(); 
      if (*bin++ != 'z') 
      { 
        // unknown compression method 
        /// \todo log this 
      } 
      else 
      { 
        size_t new_size = id3::parseNumber(bin);
        bin += sizeof(uint32);
        size_t old_size = f->GetDataSize() - sizeof(uint32) - 1;
        id3::CompressedMemoryReader cmr(bin, old_size, new_size);
        ID3_ParseFrames(tag, cmr);
        if (cmr.getEnd() != cmr.getCur())
        {
          // hmm.  it didn't parse the entire uncompressed data.  wonder why. 
          /// \todo log this. 
        } 
      } 
    } 
  } 
  if (wr.peekChar() == '\0')
  {
    ID3D_NOTICE( "ID3_ParseFrames: done parsing, padding at postion " << wr.getCur() );
  }
  else
  {
    ID3D_NOTICE( "ID3_ParseFrames: done parsing, [cur, end] = [" << wr.getCur() << ", " << wr.getEnd() << "]" );
  }
} 

void ID3_Tag::Parse(ID3_Reader& reader)
{
  ID3_Reader::pos_type beg = reader.getCur();
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): rdr.getCur() = " << beg );
  
  id3::WindowedReader wr(reader);
  wr.setWindow(beg, ID3_TagHeader::SIZE);
  wr.setCur(wr.getBeg());
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): before hdr, beg = " << wr.getBeg() );
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): before hdr, cur = " << wr.getCur() );
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): before hdr, end = " << wr.getEnd() );
  _hdr.Parse(wr);
  ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): after hdr,  cur = " << (int) wr.getCur() );

  if (wr.getCur() == beg)
  {
    return;
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
    ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): unsync beg = " << ur.getBeg() );
    ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): unsync cur = " << ur.getCur() );
    ID3D_NOTICE( "ID3_Tag::Parse(ID3_Reader&): unsync end = " << ur.getEnd() );
    ID3_ParseFrames(*this, ur);
  }
  else
  {
    ID3_ParseFrames(*this, wr);
  }

  // set the flag which says that the tag hasn't changed
  _changed = false;
}

size_t ID3_ParseFrames(ID3_Tag& tag, const uchar* const data, size_t size) 
{ 
  const uchar* const data_end = data + size; 
  size_t total_size = 0; 
  size_t frame_size = 0; 
  for (const uchar* p = data; p < data_end && *p != '\0'; p += frame_size) 
  { 
    ID3_Frame* f = new ID3_Frame; 
    f->SetSpec(tag.GetSpec());
    frame_size = f->Parse(p, data_end - p); 
    total_size += frame_size; 
     
    if (f->BadParse()) 
    { 
      // bad parse!  we can't attach this frame.  should probably log this 
      // TODO: log this 
      delete f; 
    } 
    else if (!frame_size) 
    { 
      // There is a problem. 
      // If the frame size is 0, then we can't progress. 
      // TODO: log this 
      delete f; 
      // Break for now. 
      break; 
    } 
    else if (f->GetID() != ID3FID_METACOMPRESSION) 
    { 
      // a good, uncompressed frame.  attach away! 
      tag.AttachFrame(f); 
    } 
    else 
    { 
      // hmm.  an ID3v2.2.1 compressed frame.  It contains 1 or more compressed
      // frames.  Uncompress and call ParseFrames recursively.
      const uchar* const bin = f->Field(ID3FN_DATA).GetBinary(); 
      if (*bin != 'z') 
      { 
        // unknown compression method 
        // TODO: log this 
      } 
      else 
      { 
        uint32 new_size = id3::parseNumber(&bin[1]); 
         
        uchar* uncompressed = new uchar[new_size]; 
         
        uncompress(uncompressed, (luint *) &new_size, 
                   &bin[1 + sizeof(uint32)], 
                   f->GetDataSize() - sizeof(uint32) - 1); 

        if (ID3_ParseFrames(tag, uncompressed, new_size) != new_size) 
        { 
          // hmm.  it didn't parse the entire uncompressed data.  wonder why. 
          // TODO: log this. 
        } 
         
        delete [] uncompressed; 
      } 
    } 
  } 
   
  return total_size; 
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
  id3::bstring buf(header, ID3_TagHeader::SIZE);
  buf.append(buffer, size);

  id3::MemoryReader mr(reinterpret_cast<const char *>(buf.data()), buf.size());
  ID3_Reader::pos_type beg = mr.getCur();
  this->Parse(mr);
  return mr.getCur() - beg;

  // set the flag which says that the tag hasn't changed
  _changed = false;
}

size_t ParseID3v2(ID3_Tag& tag, ifstream& file)
{
  size_t size = 0;

  if (!file)
  {
    return 0;
  }
  id3::IStreamReader isr(file);
  ID3D_NOTICE( "ParseID3v2: Created istreamreader, cur = " << isr.getCur() );
  ID3_Reader::pos_type beg = isr.getCur();
  tag.Parse(isr);
  ID3_Reader::pos_type end = isr.getCur();
  return end - beg;
}

size_t ParseMusicMatch(ID3_Tag&, ifstream&);
size_t ParseLyrics3(ID3_Tag&, ifstream&);
size_t ParseLyrics3v2(ID3_Tag&, ifstream&);
size_t ParseID3v1(ID3_Tag&, ifstream&);

void ID3_Tag::ParseFile()
{
  ifstream file;
  if (ID3E_NoError != id3::openReadableFile(this->GetFileName(), file))
  {
    // log this...
    return;
  }

  size_t bytes = 0;
  _file_tags.clear();
  _prepended_bytes = 0;
  _file_size = id3::getFileSize(file);
  do
  {
    file.seekg(_prepended_bytes, ios::beg);
    bytes = 0;

    // Parse tags at the beginning of the file first...
    if (_tags_to_parse.test(ID3TT_ID3V2))
    {
      bytes = ParseID3v2(*this, file);
      if (bytes)
      {
        // say we have v2 tags
        _file_tags.add(ID3TT_ID3V2);
      }
    }
    _prepended_bytes += bytes;
  } while (bytes);
  
  _appended_bytes = 0;
  do
  {
    bytes = 0;
    // ...then the tags at the end
    if (!bytes && _tags_to_parse.test(ID3TT_MUSICMATCH))
    {
      file.seekg(- static_cast<long>(_appended_bytes), ios::end);
      bytes = ParseMusicMatch(*this, file);
      if (bytes)
      {
        _file_tags.add(ID3TT_MUSICMATCH);
      }
    }
    if (!bytes && _tags_to_parse.test(ID3TT_LYRICS3))
    {
      file.seekg(- static_cast<long>(_appended_bytes), ios::end);
      bytes = ParseLyrics3(*this, file);
      if (bytes)
      {
        _file_tags.add(ID3TT_LYRICS3);
      }
    }
    if (!bytes && _tags_to_parse.test(ID3TT_LYRICS3V2))
    {
      file.seekg(- static_cast<long>(_appended_bytes), ios::end);
      bytes = ParseLyrics3v2(*this, file);
      if (bytes)
      {
        _file_tags.add(ID3TT_LYRICS3V2);
      }
    }
    if (!bytes && _tags_to_parse.test(ID3TT_ID3V1))
    {
      file.seekg(- static_cast<long>(_appended_bytes), ios::end);
      bytes = ParseID3v1(*this, file);
      if (bytes)
      {
        _file_tags.add(ID3TT_ID3V1);
      }
    }
    _appended_bytes += bytes;
  } while (bytes);
}
