// $Id: frame_parse.cpp,v 1.20 2000/09/30 22:12:55 eldamitri Exp $

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

#include <zlib.h>

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include "debug.h"

#include "frame.h"
#include "utils.h"
#include "reader_decorators.h"
#include "readers.h"

size_t ID3_Frame::Parse(const uchar * const buffer, size_t size) 
{ 
  id3::MemoryReader mr(reinterpret_cast<const char *>(buffer), size);
  ID3_Reader::pos_type beg = mr.getCur();
  this->Parse(mr);
  return mr.getCur() - beg;
}

namespace id3
{
  bool parseFields(ID3_Reader& rdr, ID3_Frame& frame)
  {
    ExitTrigger et(rdr);
    ID3_TextEnc enc = ID3TE_ASCII;  // set the default encoding 
    ID3_V2Spec spec = frame.GetSpec(); 
    // parse the frame's fields  
    ID3D_NOTICE( "ID3_Frame::Parse(): num_fields = " << frame.GetNumFields() );
    for (size_t i = 0; i < frame.GetNumFields(); ++i)
    {
      if (rdr.atEnd())
      { 
        // there's no remaining data to parse! 
        ID3D_WARNING( "ID3_Frame::Parse(): frame is out of data at postion " <<
                      rdr.getCur() );
        return false;
      } 
      
      ID3D_NOTICE( "ID3_Frame::Parse(): field #" << i );
      ID3_Field* fp = frame.GetFieldAt(i);
      if (NULL == fp)
      {
        // Ack!  Why is the field NULL?  Log this...
        ID3D_WARNING( "ID3_Frame::Parse(): field is null" );
        continue;
      }
      
      if (!fp->InScope(spec)) 
      {
        ID3D_NOTICE( "ID3_Frame::Parse(): field is not in scope" );
        // continue with the rest of the fields
        continue; 
      }
      
      fp->SetEncoding(enc);
      ID3_Reader::pos_type beg = rdr.getCur();
      et.setExitPos(beg);
      ID3D_NOTICE( "ID3_Frame::Parse(): parsing field, cur = " << beg );
      ID3D_NOTICE( "ID3_Frame::Parse(): parsing field, end = " << rdr.getEnd() );
      if (!fp->Parse(rdr) || rdr.getCur() == beg) 
      { 
        // nothing to parse!  ack!  parse error... 
        ID3D_WARNING( "ID3_Frame::Parse(): no data parsed, bad parse" );
        return false;
      }
      
      if (fp->GetID() == ID3FN_TEXTENC)  
      {
        enc = static_cast<ID3_TextEnc>(fp->Get());  
        ID3D_NOTICE( "ID3_Frame::Parse(): found encoding = " << enc );
      }
    }
    et.setExitPos(rdr.getCur());
  }
};

bool ID3_Frame::Parse(ID3_Reader& reader) 
{ 
  id3::ExitTrigger et(reader);
  ID3D_NOTICE( "ID3_Frame::Parse(): reader.getBeg() = " << reader.getBeg() );
  ID3D_NOTICE( "ID3_Frame::Parse(): reader.getCur() = " << reader.getCur() );
  ID3D_NOTICE( "ID3_Frame::Parse(): reader.getEnd() = " << reader.getEnd() );
  ID3_Reader::pos_type beg = reader.getCur();

  if (!_hdr.Parse(reader) || reader.getCur() == beg)  
  { 
    ID3D_WARNING( "ID3_Frame::Parse(): no header to parse" );
    return false; 
  }
  ID3D_NOTICE( "ID3_Frame::Parse(): after hdr, getCur() = " << reader.getCur() );
  ID3D_NOTICE( "ID3_Frame::Parse(): found frame! id = " << _hdr.GetTextID() );

  // data is the part of the frame buffer that appears after the header  
  const size_t data_size = _hdr.GetDataSize();
  ID3D_NOTICE( "ID3_Frame::Parse(): data_size = " << data_size );
  if (reader.getEnd() < beg + data_size)
  {
    ID3D_WARNING( "ID3_Frame::Parse(): not enough data to parse frame" );
    return false;
  }
  id3::WindowedReader wr(reader, data_size);
  ID3D_NOTICE( "ID3_Frame::Parse(): window getBeg() = " << wr.getBeg() );
  ID3D_NOTICE( "ID3_Frame::Parse(): window getCur() = " << wr.getCur() );
  ID3D_NOTICE( "ID3_Frame::Parse(): window getEnd() = " << wr.getEnd() );
  
  unsigned long expanded_size = 0;
  if (_hdr.GetCompression())
  {
    id3::NumberReader nr(wr);
    expanded_size = nr.readNumber(sizeof(uint32));
    ID3D_NOTICE( "ID3_Frame::Parse(): frame is compressed, expanded_size = " << expanded_size );
  }

  if (_hdr.GetEncryption())
  {
    char ch = wr.readChar();
    this->_SetEncryptionID(ch);
    ID3D_NOTICE( "ID3_Frame::Parse(): frame is encrypted, encryption_id = " << (int) ch );
  }

  if (_hdr.GetGrouping())
  {
    char ch = wr.readChar();
    this->_SetGroupingID(ch);
    ID3D_NOTICE( "ID3_Frame::Parse(): frame is encrypted, grouping_id = " << (int) ch );
  }

  // set the type of frame based on the parsed header  
  this->_ClearFields(); 
  this->_InitFields(); 

  bool success = false;
  // expand out the data if it's compressed 
  if (!_hdr.GetCompression())
  {
    success = id3::parseFields(wr, *this);
  }
  else
  {
    id3::CompressedStreamReader csr(wr, expanded_size);
    success = id3::parseFields(csr, *this);
  }
  et.setExitPos(wr.getCur());

  _changed = false;
  return true;
} 
