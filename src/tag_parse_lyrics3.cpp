// $Id: tag_parse_lyrics3.cpp,v 1.22 2000/09/30 22:13:52 eldamitri Exp $

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
#include "tag.h"
#include "utils.h"
#include "misc_support.h"
#include "reader_decorators.h"
#include "readers.h"

namespace id3
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
      ID3D_NOTICE( "id3::NumberStringReader::readInteger: val = " << val );
      return val;
    }
  };

  class FinderReader : public IdentityReader
  {
    typedef IdentityReader SUPER;
  public:
    FinderReader(ID3_Reader& reader) : SUPER(reader) { ; }
    virtual ~FinderReader() { ; }

    bool findText(id3::string text)
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
          ID3D_NOTICE( "id3::NumberStringReader::readInteger: found \"" << text << "\" at " << this->getCur() );
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
        '[' == this->readChar() && 
        isdigit(this->readChar()) && isdigit(this->readChar()) &&
        ':' == this->readChar() && 
        isdigit(this->readChar()) && isdigit(this->readChar()) &&
        ']' == this->readChar();
      this->setCur(cur);
      if (its)
        ID3D_NOTICE( "id3::TimeStampReader::isTimeStamp(): found timestamp, cur = " << this->getCur() );
      return its;
    }
    
    uint32 readTimeStamp()
    {
      this->skipChars(1);
      NumberStringReader nsr(*this);
      size_t sec = nsr.readInteger(2) * 60;
      this->skipChars(1);
      sec += nsr.readInteger(2);
      this->skipChars(1);
      ID3D_NOTICE( "TimeStampReader::readTimeStamp(): timestamp = " << sec );
      return sec * 1000;
    }

  };

  id3::string lyrics3ToSylt(ID3_Reader& rdr)
  {
    id3::string dest;
    while (!rdr.atEnd())
    {
      bool lf = false;
      size_t ms = 0;
      size_t count = 0;
      TimeStampReader tsr(rdr);
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
      ID3D_NOTICE( "id3::lyrics3toSylt: ms = " << ms );
      
      dest += renderNumber(ms, sizeof(uint32));
      if (lf)
      {
        ID3D_NOTICE( "id3::lyrics3toSylt: adding lf" );
    
        // put the LF
        dest += 0x0A;
      }
    }

    return dest;
  }

  bool parseLyrics3(ID3_Tag& tag, ID3_Reader& reader)
  {
    ID3_Reader::pos_type end = reader.getCur();
    if (end < 9 + 128)
    {
      ID3D_NOTICE( "id3::parseLyrics3: bailing, not enough bytes to parse, pos = " << end );
      return false;
    }
    reader.setCur(end - (9 + 128));
    id3::string lyr3id, v1id;

    {
      TextReader tr(reader);

      lyr3id = tr.readText(9);
      v1id   = tr.readText(3);
    }

    // first check for an ID3v1 tag
    if (v1id != "TAG")
    {
      reader.setCur(end);
      return false;
    }
    
    // check for lyrics
    if (lyr3id != "LYRICSEND")
    {
      reader.setCur(end);
      return false;
    }
    
    // we have a Lyrics3 v1.00 tag
    if (end < reader.getBeg() + 11 + 9 + 128)
    {
      // the file size isn't large enough to actually hold lyrics
      ID3D_WARNING( "id3::parseLyrics3: not enough data to parse lyrics3" );
      reader.setCur(end);
      return false;
    }
    
    // reserve enough space for lyrics3 + id3v1 tag
    size_t window = end - reader.getBeg();
    size_t lyrDataSize = min<size_t>(window, 11 + 5100 + 9 + 128);
    reader.setCur(end - lyrDataSize);
    ID3_Reader::pos_type beg = reader.getCur();

    WindowedReader wr(reader);
    wr.setBeg(beg);
    wr.setEnd(end - (9 + 128));
    
    FinderReader fr(wr);
    if (fr.findText("LYRICSBEGIN"))
    {
      beg = fr.getCur();
      fr.skipChars(11);
    }
    else
    {
      ID3D_WARNING( "id3::parseLyrics3: couldn't find LYRICSBEGIN, bailing" );
      reader.setCur(end);
      return false;
    }
    
    wr.setBeg(wr.getCur());
    LineFeedReader lfr(wr);
    TextReader tr(lfr);
    id3::string lyrics = tr.readText();
    
    ID3_AddLyrics(&tag, lyrics.c_str(), "Converted from Lyrics3 v1.00");
    reader.setCur(beg);
    return true;
  }



  bool parseLyrics3v2(ID3_Tag& tag, ID3_Reader& reader)
  {
    ID3_Reader::pos_type end = reader.getCur();
    if (end < 6 + 9 + 128)
    {
      ID3D_NOTICE( "id3::parseLyrics3v2: bailing, not enough bytes to parse, pos = " << end );
      reader.setCur(end);
      return false;
    }

    reader.setCur(end - (6 + 9 + 128));
    id3::string lyr3id, v1id;
    uint32 lyrSize = 0;

    {
      TextReader tr(reader);
      NumberStringReader snr(tr);
      lyrSize = snr.readInteger(6);
      lyr3id  =  tr.readText(9);
      v1id    =  tr.readText(3);
    }
  
    // first check for an ID3v1 tag
    if (v1id != "TAG")
    {
      reader.setCur(end);
      return false;
    }
     
    if (lyr3id != "LYRICS200")
    {
      reader.setCur(end);
      return false;
    }

    if (end < reader.getBeg() + lyrSize + 6 + 9 + 128)
    {
      ID3D_WARNING( "id3::parseLyrics3v2: not enough data to parse tag, lyrSize = " << lyrSize );
      reader.setCur(end);
      return false;
    }
    reader.setCur(end - (lyrSize + 6 + 9 + 128));
    
    WindowedReader wr(reader);
    wr.setWindow(wr.getCur(), lyrSize);
    
    ID3_Reader::pos_type beg     = wr.getCur();
    
    {
      TextReader tr(wr);
      id3::string          beginid = tr.readText(11);
    
      if (beginid != "LYRICSBEGIN")
      {
        // not a lyrics v2.00 tag
        ID3D_WARNING( "id3::parseLyrics3v2: couldn't find LYRICSBEGIN, bailing, beginid = " << beginid );
        tr.setCur(end);
        return false;
      }
    }
    
    bool has_time_stamps = false;
    
    ID3_Frame *lyr_frame = NULL;
    
    while (!wr.atEnd())
    {
      string fldName, fldData;
      uint32 fldSize;

      {
        TextReader tr(wr);
        fldName = tr.readText(3);
        ID3D_NOTICE( "id3::parseLyrics3v2: fldName = " << fldName );
        NumberStringReader snr(tr);
        fldSize = snr.readInteger(5);
        ID3D_NOTICE( "id3::parseLyrics3v2: fldSize = " << fldSize );
      }

      {
        WindowedReader wr2(wr);
        wr2.setWindow(wr.getCur(), fldSize);
        LineFeedReader lfr(wr2);
        TextReader tr(lfr);
      
        fldData = tr.readText(fldSize);
        ID3D_NOTICE( "id3::parseLyrics3v2: fldData = \"" << fldData << "\"" );
      }

      // the IND field
      if (fldName == "IND")
      {
        has_time_stamps = (fldData.size() > 1 && fldData[1] == '1');
      }
      
      // the TITLE field
      else if (fldName == "ETT")
      {
        ID3_AddTitle(&tag, fldData.c_str());
      }
      
      // the ARTIST field
      else if (fldName == "EAR")
      {
        ID3_AddArtist(&tag, fldData.c_str());
      }
      
      // the ALBUM field
      else if (fldName == "EAL")
      {
        ID3_AddAlbum(&tag, fldData.c_str(), true);
      }
      
      // the Lyrics/Music AUTHOR field
      else if (fldName == "AUT")
      {
        ID3_AddLyricist(&tag, fldData.c_str());
      }
      
      // the INFORMATION field
      else if (fldName == "INF")
      {
        ID3_AddComment(&tag, fldData.c_str(), "Lyrics3 v2.00 INF");
      }
      
      // the LYRICS field
      else if (fldName == "LYR")
      {
        // if already found an INF field, use it as description
        const char* desc =  "Converted from Lyrics3 v2.00";
        if (!has_time_stamps)
        {
          lyr_frame = ID3_AddLyrics(&tag, fldData.c_str(), desc);
        }
        else
        {
          // converts from lyrics3 to SYLT in-place
          MemoryReader mr(fldData.data(), fldData.size());
          ID3D_NOTICE( "id3::parseLyrics3v2: determining synced lyrics" );
          id3::string sylt = lyrics3ToSylt (mr);

          lyr_frame = ID3_AddSyncLyrics(&tag, reinterpret_cast<const uchar*>(sylt.c_str()), sylt.size(), ID3TSF_MS, desc);
          ID3D_NOTICE( "id3::parseLyrics3v2: determined synced lyrics" );
        }
      }
      else if (fldName == "IMG")
      {
        // currently unsupported
        ID3D_WARNING( "id3::parseLyrics3v2: IMG tag unsupported" );
      }
      else
      {
        ID3D_WARNING( "id3::parseLyrics3v2: undefined field id: " << fldName );
      }
    }
    reader.setCur(beg);
    return true;
  }
}
