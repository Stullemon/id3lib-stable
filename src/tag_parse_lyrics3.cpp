// $Id: tag_parse_lyrics3.cpp,v 1.24 2000/10/09 04:30:29 eldamitri Exp $

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
#include <ctype.h>
#include <memory.h>
#include "tag_impl.h"
#include "utils.h"
#include "helpers.h"
#include "reader_decorators.h"
#include "readers.h"

namespace dami
{
  namespace io
  {
    class NumberStringReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
    public:
      NumberStringReader(ID3_Reader& reader) : SUPER(reader) { ; }
      virtual ~NumberStringReader() { ; }
      
      uint32 readInteger()
      {
        return this->readInteger(this->getEnd() - this->getCur());
      }
      
      uint32 readInteger(size_t numBytes)
      {
        uint32 val = 0;
        for (size_t i = 0; i < numBytes && isdigit(this->peekChar()); ++i)
        {
          val = (val * 10) + (this->readChar() - '0');
        }
        ID3D_NOTICE( "io::NumberStringReader::readInteger: val = " << val );
        return val;
      }
    };
    
    class FinderReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
    public:
      FinderReader(ID3_Reader& reader) : SUPER(reader) { ; }
      virtual ~FinderReader() { ; }
      
      bool findText(String text)
      {
        if (text.empty())
        {
          return true;
        }
        
        size_t index = 0;
        while (!this->atEnd())
        {
          char_type ch = this->readChar();
          if (ch == text[index])
          {
            index++;
          }
          else if (ch = text[0])
          {
            index = 1;
          }
          else
          {
            index = 0;
          }
          if (index == text.size())
          {
            this->setCur(this->getCur() - index);
            ID3D_NOTICE( "io::FinderReader::readInteger: found \"" << text << "\" at " << this->getCur() );
            break;
          }
        }
        return !this->atEnd();
      };
    };
    
    class TimeStampReader : public IdentityReader
    {
      typedef IdentityReader SUPER;
    public:
      TimeStampReader(ID3_Reader& rdr) : SUPER(rdr) { ; }
      virtual ~TimeStampReader() { ; }
      
      bool isTimeStamp()
      {
        pos_type cur = this->getCur();
        if (this->getEnd() < cur + 7)
        {
          return false;
        }
        bool its = 
        ('[' == this->readChar() && 
         isdigit(this->readChar()) && isdigit(this->readChar()) &&
         ':' == this->readChar() && 
         isdigit(this->readChar()) && isdigit(this->readChar()) &&
         ']' == this->readChar());
        this->setCur(cur);
        if (its)
          ID3D_NOTICE( "io::TimeStampReader::isTimeStamp(): found timestamp, cur = " << this->getCur() );
        return its;
      }
      
      uint32 readTimeStamp()
      {
        this->skipChars(1);
        io::NumberStringReader nsr(*this);
        size_t sec = nsr.readInteger(2) * 60;
        this->skipChars(1);
        sec += nsr.readInteger(2);
        this->skipChars(1);
        ID3D_NOTICE( "TimeStampReader::readTimeStamp(): timestamp = " << sec );
        return sec * 1000;
      }
    };
  };
};

using namespace dami;

namespace
{
  String lyrics3ToSylt(ID3_Reader& rdr)
  {
    String dest;
    while (!rdr.atEnd())
    {
      bool lf = false;
      size_t ms = 0;
      size_t count = 0;
      io::TimeStampReader tsr(rdr);
      while (tsr.isTimeStamp())
      {
        // For now, just skip over multiple time stamps
        if (count++ > 0)
        {
          tsr.readTimeStamp();
        }
        else
        {
          ms = tsr.readTimeStamp();
        }
      }
      while (!tsr.atEnd() && !tsr.isTimeStamp())
      {
        ID3_Reader::char_type ch = tsr.readChar();
        if (0x0A == ch && (tsr.atEnd() || tsr.isTimeStamp()))
        {
          lf = true;
          break;
        }
        else
        {
          dest += static_cast<char>(ch);
        }
      }
      
      // put synch identifier
      dest += '\0';
      
      // put timestamp
      ID3D_NOTICE( "lyrics3toSylt: ms = " << ms );
      
      dest += renderNumber(ms, sizeof(uint32));
      if (lf)
      {
        ID3D_NOTICE( "lyrics3toSylt: adding lf" );
    
        // put the LF
        dest += 0x0A;
      }
    }

    return dest;
  }
};

//bool parse(TagImpl& tag, ID3_Reader& reader)
bool lyr3::v1::parse(ID3_TagImpl& tag, ID3_Reader& reader)
{
  io::ExitTrigger et(reader);
  ID3_Reader::pos_type end = reader.getCur();
  if (end < reader.getBeg() + 9 + 128)
  {
    ID3D_NOTICE( "id3::v1::parse: bailing, not enough bytes to parse, pos = " << end );
    return false;
  }
  reader.setCur(end - (9 + 128));
        
  {
    io::TextReader tr(reader);
          
    if (tr.readText(9) != "LYRICSEND" || tr.readText(3) != "TAG")
    {
      return false;
    }
  }
        
  // we have a Lyrics3 v1.00 tag
  if (end < reader.getBeg() + 11 + 9 + 128)
  {
    // the file size isn't large enough to actually hold lyrics
    ID3D_WARNING( "id3::v1::parse: not enough data to parse lyrics3" );
    return false;
  }
        
  // reserve enough space for lyrics3 + id3v1 tag
  size_t window = end - reader.getBeg();
  size_t lyrDataSize = min<size_t>(window, 11 + 5100 + 9 + 128);
  reader.setCur(end - lyrDataSize);
  io::WindowedReader wr(reader, lyrDataSize - (9 + 128));
        
  {
    io::FinderReader fr(wr);
    if (!fr.findText("LYRICSBEGIN"))
    {
      ID3D_WARNING( "id3::v1::parse: couldn't find LYRICSBEGIN, bailing" );
      return false;
    }
  }
        
  et.setExitPos(wr.getCur());
  wr.skipChars(11);
  wr.setBeg(wr.getCur());

  {
    io::LineFeedReader lfr(wr);
    io::TextReader tr(lfr);
    String lyrics = tr.readText();
    id3::v2::setLyrics(tag, lyrics, "Converted from Lyrics3 v1.00", "XXX");
  }

  return true;
}
    
//bool parse(TagImpl& tag, ID3_Reader& reader)
bool lyr3::v2::parse(ID3_TagImpl& tag, ID3_Reader& reader)
{
  io::ExitTrigger et(reader);
  ID3_Reader::pos_type end = reader.getCur();
  if (end < reader.getBeg() + 6 + 9 + 128)
  {
    ID3D_NOTICE( "lyr3::v2::parse: bailing, not enough bytes to parse, pos = " << reader.getCur() );
    return false;
  }
        
  reader.setCur(end - (6 + 9 + 128));
  uint32 lyrSize = 0;

  {
    io::NumberStringReader nsr(reader);
    ID3_Reader::pos_type beg = nsr.getCur();
    lyrSize = nsr.readInteger(6);
    if (nsr.getCur() < beg + 6)
    {
      ID3D_NOTICE( "lyr3::v2::parse: couldn't find numeric string, lyrSize = " << lyrSize );
      return false;
    }
  }
    
  {
    io::TextReader tr(reader);
    if (tr.readText(9) != "LYRICS200" || tr.readText(3) != "TAG")
    {
      return false;
    }
  }
        
  if (end < reader.getBeg() + lyrSize + 6 + 9 + 128)
  {
    ID3D_WARNING( "lyr3::v2::parse: not enough data to parse tag, lyrSize = " << lyrSize );
    return false;
  }
  reader.setCur(end - (lyrSize + 6 + 9 + 128));
        
  io::WindowedReader wr(reader);
  wr.setWindow(wr.getCur(), lyrSize);
        
  ID3_Reader::pos_type beg     = wr.getCur();
        
  {
    io::TextReader tr(wr);
    String          beginid = tr.readText(11);
          
    if (beginid != "LYRICSBEGIN")
    {
      // not a lyrics v2.00 tag
      ID3D_WARNING( "lyr3::v2::parse: couldn't find LYRICSBEGIN, bailing, beginid = " << beginid );
      return false;
    }
  }
        
  bool has_time_stamps = false;
        
  ID3_Frame* lyr_frame = NULL;
        
  while (!wr.atEnd())
  {
    string fldName, fldData;
    uint32 fldSize;
          
    {
      io::TextReader tr(wr);
      fldName = tr.readText(3);
      ID3D_NOTICE( "lyr3::v2::parse: fldName = " << fldName );
      io::NumberStringReader snr(tr);
      fldSize = snr.readInteger(5);
      ID3D_NOTICE( "lyr3::v2::parse: fldSize = " << fldSize );
    }
          
    {
      io::WindowedReader wr2(wr);
      wr2.setWindow(wr.getCur(), fldSize);
      io::LineFeedReader lfr(wr2);
      io::TextReader tr(lfr);
            
      fldData = tr.readText(fldSize);
      ID3D_NOTICE( "lyr3::v2::parse: fldData = \"" << fldData << "\"" );
    }
          
    // the IND field
    if (fldName == "IND")
    {
      has_time_stamps = (fldData.size() > 1 && fldData[1] == '1');
    }
          
    // the TITLE field
    else if (fldName == "ETT")
    {
      //tag.setTitle(fldData);
      id3::v2::setTitle(tag, fldData);
    }
          
    // the ARTIST field
    else if (fldName == "EAR")
    {
      //tag.setArtist(fldData);
      id3::v2::setArtist(tag, fldData);
    }
          
    // the ALBUM field
    else if (fldName == "EAL")
    {
      //tag.setAlbum(fldData);
      id3::v2::setArtist(tag, fldData);
    }
          
    // the Lyrics/Music AUTHOR field
    else if (fldName == "AUT")
    {
      //tag.setAuthor(fldData);
      id3::v2::setLyricist(tag, fldData);
    }
          
    // the INFORMATION field
    else if (fldName == "INF")
    {
      //tag.setInfo(fldData);
      id3::v2::setComment(tag, fldData, "Lyrics3 v2.00 INF", "XXX");
    }
          
    // the LYRICS field
    else if (fldName == "LYR")
    {
      // if already found an INF field, use it as description
      String desc =  "Converted from Lyrics3 v2.00";
      //tag.setLyrics(fldData);
      if (!has_time_stamps)
      {
        lyr_frame = id3::v2::setLyrics(tag, fldData, desc, "XXX");
      }
      else
      {
        // converts from lyrics3 to SYLT in-place
        ID3_MemoryReader mr(fldData.data(), fldData.size());
        ID3D_NOTICE( "lyr3::v2::parse: determining synced lyrics" );
        String sylt = lyrics3ToSylt (mr);
              
        lyr_frame = id3::v2::setSyncLyrics(tag, sylt, ID3TSF_MS, desc,
                                           "XXX", ID3CT_LYRICS);
        ID3D_NOTICE( "lyr3::v2::parse: determined synced lyrics" );
      }
    }
    else if (fldName == "IMG")
    {
      // currently unsupported
      ID3D_WARNING( "lyr3::v2::parse: IMG field unsupported" );
    }
    else
    {
      ID3D_WARNING( "lyr3::v2::parse: undefined field id: " << 
                    fldName );
    }
  }

  et.setExitPos(beg);
  return true;
}
