// $Id: frame_parse.cpp,v 1.19 2000/09/27 08:30:08 eldamitri Exp $

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

void ID3_Frame::Parse(ID3_Reader& reader) 
{ 
  ID3D_NOTICE( "ID3_Frame::Parse(): reader.getBeg() = " << reader.getBeg() );
  ID3D_NOTICE( "ID3_Frame::Parse(): reader.getCur() = " << reader.getCur() );
  ID3D_NOTICE( "ID3_Frame::Parse(): reader.getEnd() = " << reader.getEnd() );
  ID3_Reader::pos_type beg = reader.getCur();
  _bad_parse = false;  
  _hdr.Parse(reader);  
  ID3D_NOTICE( "ID3_Frame::Parse(): after hdr, getCur() = " << reader.getCur() );

  if (reader.getCur() == beg)  
  { 
    ID3D_WARNING( "ID3_Frame::Parse(): hdr size is 0" );
    reader.setCur(beg);
    return;  
  }  
  ID3D_NOTICE( "ID3_Frame::Parse(): found frame! id = " << _hdr.GetTextID() );

  // data is the part of the frame buffer that appears after the header  
  const size_t data_size = _hdr.GetDataSize();
  ID3D_NOTICE( "ID3_Frame::Parse(): data_size = " << data_size );
  id3::WindowedReader wr(reader);
  wr.setWindow(wr.getCur(), data_size);
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

  ID3_Reader* rdr;

  // expand out the data if it's compressed 
  if (!_hdr.GetCompression())
  {
    rdr = &wr;
  }
  else
  {
    rdr = new id3::CompressedStreamReader(wr, expanded_size);
  }

  // set the type of frame based on the parsed header  
  this->_ClearFields(); 
  this->_InitFields(); 

  ID3_TextEnc enc = ID3TE_ASCII;  // set the default encoding 
  ID3_V2Spec spec = this->GetSpec(); 
  // parse the frame's fields  
  ID3D_NOTICE( "ID3_Frame::Parse(): num_fields = " << _num_fields );
  for (ID3_Field** fi = _fields; fi != _fields + _num_fields; fi++)
  {
    size_t field_num = fi - _fields;
    ID3D_NOTICE( "ID3_Frame::Parse(): field #" << field_num );
    if (!*fi)
    {
      // Ack!  Why is the field NULL?  Log this...
      ID3D_WARNING( "ID3_Frame::Parse(): field is null" );
      continue;
    }

    if (rdr->peekChar() == ID3_Reader::END_OF_READER) 
    { 
      // there's no remaining data to parse! 
      ID3D_WARNING( "ID3_Frame::Parse(): frame is out of data at postion " << rdr->getCur() );
      _bad_parse = true; 
      break; 
    } 
 
    if (!(*fi)->InScope(spec)) 
    {
      ID3D_NOTICE( "ID3_Frame::Parse(): field is not in scope" );
      // continue with the rest of the fields
      continue; 
    }
      
    (*fi)->SetEncoding(enc);
    beg = rdr->getCur();
    ID3D_NOTICE( "ID3_Frame::Parse(): parsing field, cur = " << beg );
    ID3D_NOTICE( "ID3_Frame::Parse(): parsing field, end = " << rdr->getEnd() );
    (*fi)->Parse(*rdr);
      
    if (rdr->getCur() == beg) 
    { 
      // nothing to parse!  ack!  parse error... 
      ID3D_WARNING( "ID3_Frame::Parse(): no data parsed, bad parse" );
      _bad_parse = true; 
      break;
    } 
 
    if ((*fi)->GetID() == ID3FN_TEXTENC)  
    {
      enc = static_cast<ID3_TextEnc>((*fi)->Get());  
      ID3D_NOTICE( "ID3_Frame::Parse(): found encoding = " << enc );
    }
  }  
      
  _changed = false;

  if (rdr != &wr)
  {
    delete rdr;
  }
} 
