// $Id: tag_parse_musicmatch.cpp,v 1.11 2000/10/03 04:51:27 eldamitri Exp $

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

#include <stdlib.h>
#include "tag_impl.h"
#include "utils.h"
#include "helpers.h"
#include "reader_decorators.h"

namespace dami
{
  namespace io
  {
    class LENumberReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
    public:
      LENumberReader(ID3_Reader& reader) : SUPER(reader) { ; }
      virtual ~LENumberReader() { ; }

      uint32 readInteger()
      {
        return this->readInteger(this->getEnd() - this->getCur());
      }
      
      uint32 readInteger(size_t numBytes)
      {
        uint32 val = 0;
        for (size_t i = 0; i < numBytes; i++)
        {
          if (this->atEnd())
          {
            break;
          }
          val += (static_cast<size_t>(0xFF & this->readChar()) << (i * 8));
        }
        return val;
      }
    };

    class TimeReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
    public:
      TimeReader(ID3_Reader& reader) : SUPER(reader) { ; }
      virtual ~TimeReader() { ; }

      uint32 readSeconds(size_t numBytes)
      {
        io::WindowedReader wr(*this, numBytes);
        pos_type beg = wr.getCur();
        uint32 seconds = 0;
        uint32 cur = 0;
        while (!wr.atEnd())
        {
          char_type ch = wr.readChar();
          if (':' == ch)
          {
            seconds += 60 * cur;
            cur = 0;
          }
          else if (!isdigit(ch))
          {
            this->setCur(beg);
            return 0;
          }
          else
          {
            cur = cur * 10 + (ch - '0');
          }
        }
        return seconds + cur;
      }
    };

    class TextFrameReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
    public:
      TextFrameReader(ID3_Reader& reader) : SUPER(reader) { ; }
      virtual ~TextFrameReader() { ; }

      ID3_Frame* readFrame(ID3_FrameID id, const String desc = "")
      {
        LENumberReader nr(*this);
        uint32 size = nr.readInteger(2);
        ID3D_NOTICE( "io::TextFrameReader::readFrame() size = " << size );
        if (size == 0)
        {
          return NULL;
        }

        String text;
        if (ID3FID_SONGLEN != id)
        {
          LineFeedReader lfr(*this);
          TextReader tr(lfr);
          text = tr.readText(size);
          ID3D_NOTICE( "io::TextFrameReader::readFrame() text = " << text );
        }
        else
        {
          TimeReader tr(*this);
          text = toString(tr.readSeconds(size));
          ID3D_NOTICE( "io::TextFrameReader::readFrame() songlen = " << text );
        }
        //wr.setCur(this->getEnd());

        ID3_Frame* frame = new ID3_Frame(id);
        if (frame)
        {
          if (frame->Contains(ID3FN_TEXT))
          {
            frame->Field(ID3FN_TEXT).Set(text.c_str());
          }
          else if (frame->Contains(ID3FN_URL))
          {
            frame->Field(ID3FN_URL).Set(text.c_str());
          }
          if (frame->Contains(ID3FN_LANGUAGE))
          {
            frame->Field(ID3FN_LANGUAGE).Set("XXX");
          }
          if (frame->Contains(ID3FN_DESCRIPTION))
          {
            frame->Field(ID3FN_DESCRIPTION).Set(desc.c_str());
          }
        }
        return frame;
      }
    };
  };

  namespace mm
  {
    bool parse(ID3_TagImpl& tag, ID3_Reader& rdr);
  }
};

using namespace dami;

bool mm::parse(ID3_TagImpl& tag, ID3_Reader& rdr)
{
  ID3_Reader::pos_type end = rdr.getCur();
  io::ExitTrigger et(rdr, end);
  if (end < 48)
  {
    ID3D_NOTICE( "mm::parse: bailing, not enough bytes to parse, pos = " << end );
    return false;
  }
  
  rdr.setCur(end - 48);
  String version;
  
  {
    io::TextReader tr(rdr);
    if (tr.readText(32) != "Brava Software Inc.             ")
    {
      ID3D_NOTICE( "mm::parse: bailing, couldn't find footer" );
      return false;
    }
    
    version = tr.readText(4);
    if (version.size() != 4 || 
        !isdigit(version[0]) || version[1] != '.' ||
        !isdigit(version[2]) || 
        !isdigit(version[3]))
    {
      ID3D_WARNING( "mm::parse: bailing, nonstandard version = " << version );
      return false;
    }
  }
    
  ID3_Reader::pos_type beg = rdr.setCur(end - 48);
  et.setExitPos(beg);
  if (end < 68)
  {
    ID3D_NOTICE( "mm::parse: bailing, not enough bytes to parse offsets, pos = " << end );
    return false;
  }
  rdr.setCur(end - 68);
    
  io::WindowedReader dataWindow(rdr);
  dataWindow.setEnd(rdr.getCur());

  uint32 offsets[5];
    
  {
    io::WindowedReader offsetWindow(rdr, 20);
    io::LENumberReader offsetRdr(offsetWindow);
    for (size_t i = 0; i < 5; ++i)
    {
      offsets[i] = offsetRdr.readInteger(sizeof(uint32));
    }
  }

  size_t metadataSize = 0;
  if (version <= "3.00")
  {
    // All MusicMatch tags up to and including version 3.0 had metadata 
    // sections exactly 7868 bytes in length.
    metadataSize = 7868;
  }
  else
  {
    // MusicMatch tags after version 3.0 had three possible lengths for their
    // metadata sections.  We can determine which it was by searching for
    // the version section signature that should precede the metadata section
    // by exactly 256 bytes.
    size_t possibleSizes[] = { 8132, 8004, 7936 };
      
    for (size_t i = 0; i < sizeof(possibleSizes)/sizeof(size_t); ++i)
    {
      dataWindow.setCur(dataWindow.getEnd());
        
      // Our offset will be exactly 256 bytes prior to our potential metadata
      // section
      int offset = possibleSizes[i] + 256;
      if (dataWindow.getCur() < offset)
      {
        // if our filesize is less than the offset, then it can't possibly
        // be the correct offset, so try again.
        continue;
      }
      dataWindow.setCur(dataWindow.getCur() - offset);
      io::TextReader tr(dataWindow);
        
      // now read in the signature to see if it's a match
      if (tr.readText(8) == "18273645")
      {
        metadataSize = possibleSizes[i];
        break;
      }
    }
  }
  if (0 == metadataSize)
  {
    // if we didn't establish a size for the metadata, then something is
    // wrong.  probably should log this.
    ID3D_WARNING( "mm::parse: bailing, couldn't find meta data signature, end = " << end );
    return false;
  }
    
  // parse the offset pointers to determine the actual sizes of all the 
  // sections
  size_t sectionSizes[5];
  size_t tagSize = metadataSize;
    
  // we already know the size of the last section
  sectionSizes[4] = metadataSize;
    
  size_t lastOffset = 0;
  for (int i = 0; i < 5; i++)
  {
    size_t thisOffset = offsets[i];
    //ASSERT(thisOffset > lastOffset);
    if (i > 0)
    {
      size_t sectionSize = thisOffset - lastOffset;
      sectionSizes[i-1] = sectionSize;
      tagSize += sectionSize;
    }
    lastOffset = thisOffset;
  }
    
  // now check to see that our tag size is reasonable
  if (dataWindow.getEnd() < tagSize)
  {
    // Ack!  The tag size doesn't jive with the tag's ending position in
    // the file.  Bail!
    ID3D_WARNING( "mm::parse: bailing, tag size is too big, tag size = " << tagSize << ", end = " << end );
    return false;
  }

  dataWindow.setBeg(dataWindow.getEnd() - tagSize);
  dataWindow.setCur(dataWindow.getBeg());
    
  // Now calculate the adjusted offsets
  offsets[0] = dataWindow.getBeg();
  for (size_t i = 0; i < 4; ++i)
  {
    offsets[i+1] = offsets[i] + sectionSizes[i];
  }
    
  // now check for a tag header and adjust the tag_beg pointer appropriately
  if (dataWindow.getBeg() >= 256)
  {
    rdr.setCur(dataWindow.getBeg() - 256);
    io::TextReader tr(rdr);
    if (tr.readText(8) == "18273645")
    {
      et.setExitPos(tr.getCur() - 8);
    }
    dataWindow.setCur(dataWindow.getBeg());
  }
    
  // Now parse the various sections...
  io::TextReader tr(dataWindow);
    
  // Parse the image extension at offset 0
  dataWindow.setCur(offsets[0]);
  String imgExt = ::removeTrailingSpaces(tr.readText(4));
    
  // Parse the image binary at offset 1
  dataWindow.setCur(offsets[1]);
  io::LENumberReader nr(dataWindow);
  uint32 imgSize = nr.readInteger(4);
  if (imgSize == 0)
  {
    // no image binary.  don't do anything.
  }
  else
  {
    io::WindowedReader imgWindow(dataWindow, imgSize);
    if (imgWindow.getEnd() < imgWindow.getBeg() + imgSize)
    {
      // Ack!  The image size given extends beyond the next offset!  This is 
      // not good...  log?
    }
    else
    {
      io::TextReader dataReader(imgWindow);
      String imgData = dataReader.readText();
      ID3_Frame* frame = new ID3_Frame(ID3FID_PICTURE);
      if (frame)
      {
        String mimetype("image/");
        mimetype += imgExt;
        frame->Field(ID3FN_MIMETYPE).Set(mimetype.c_str());
        frame->Field(ID3FN_IMAGEFORMAT).Set("");
        frame->Field(ID3FN_PICTURETYPE).Set(static_cast<unsigned int>(0));
        frame->Field(ID3FN_DESCRIPTION).Set("");
        frame->Field(ID3FN_DATA).Set(reinterpret_cast<const uchar*>(imgData.data()), imgData.size());
        tag.AttachFrame(frame);
      }
    }
  }
    
  //file.seekg(offsets[2]);
  //file.seekg(offsets[3]);
  dataWindow.setCur(offsets[4]);
  io::TextFrameReader tfr(dataWindow);
    
  tag.AttachFrame(tfr.readFrame(ID3FID_TITLE));
  tag.AttachFrame(tfr.readFrame(ID3FID_ALBUM));
  tag.AttachFrame(tfr.readFrame(ID3FID_LEADARTIST));
  tag.AttachFrame(tfr.readFrame(ID3FID_CONTENTTYPE));
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Tempo"));
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Mood"));
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Situation"));
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Preference"));
  tag.AttachFrame(tfr.readFrame(ID3FID_SONGLEN));
    
  // The next 12 bytes can be ignored.  The first 8 represent the 
  // creation date as a 64 bit floating point number.  The last 4 are
  // for a play counter.
  tfr.skipChars(12);
    
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Path"));
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Serial"));
    
  // 2 bytes for track
  uint32 trkNum = nr.readInteger(2);
  if (trkNum > 0)
  {
    String trkStr = ::toString(trkNum);
    ID3_Frame* frame = new ID3_Frame(ID3FID_TRACKNUM);
    if (frame)
    {
      frame->Field(ID3FN_TEXT).Set(trkStr.c_str());
      tag.AttachFrame(frame);
    }
  }
    
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Notes"));
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_Bio"));
  tag.AttachFrame(tfr.readFrame(ID3FID_UNSYNCEDLYRICS));
  tag.AttachFrame(tfr.readFrame(ID3FID_WWWARTIST));
  tag.AttachFrame(tfr.readFrame(ID3FID_WWWCOMMERCIALINFO));
  tag.AttachFrame(tfr.readFrame(ID3FID_COMMENT, "MusicMatch_ArtistEmail"));
    
  // email?
    
  return true;
}
