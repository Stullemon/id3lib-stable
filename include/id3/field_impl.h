// -*- C++ -*- 
// $Id: field_impl.h,v 1.7 2000/10/12 22:28:06 eldamitri Exp $

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

#ifndef _ID3LIB_FIELD_IMPL_H_
#define _ID3LIB_FIELD_IMPL_H_

#include <stdlib.h>
#include "field.h"
#include "error.h"

class ID3_FieldDef;
class ID3_FrameDef;
class ID3_Frame;
class ID3_Reader;

class ID3_FieldImpl : public ID3_Field
{
  friend class ID3_FrameImpl;
public:
  ~ID3_FieldImpl();
  
  void Clear();

  size_t Size() const;
  size_t BinSize() const;
  size_t GetNumTextItems() const;

  // integer field functions
  ID3_Field&    operator= (uint32 val) { this->Set(val); return *this; }
  void          Set(uint32);
  uint32        Get() const 
  { 
    if (this->GetType() == ID3FTY_INTEGER)
    {
      return _integer; 
    }
    return 0;
  }

  // ASCII string field functions
  ID3_Field&    operator= (const char* s) { this->Set(s); return *this; }
  size_t        Set(const char*);
  size_t        Get(char*, size_t) const;
  size_t        Get(char*, size_t, index_t) const;
  const char*   GetText() const;
  const char*   GetTextItem(index_t) const;
  size_t        Add(const char*);

  // Unicode string field functions
  ID3_Field&    operator= (const unicode_t* s) { this->Set(s); return *this; }
  size_t        Set(const unicode_t*);
  size_t        Get(unicode_t *buffer, size_t) const;
  size_t        Get(unicode_t *buffer, size_t, index_t) const;
  const unicode_t* GetUnicodeText() const;
  const unicode_t* GetUnicodeTextItem(index_t) const;
  size_t        Add(const unicode_t*);

  // binary field functions
  size_t        Set(const uchar*, size_t);
  size_t        Set(const char* buf, size_t size)
  { 
    return this->Set(reinterpret_cast<const uchar *>(buf), size);
  }
  size_t        Get(uchar*, size_t) const;
  const uchar*  GetBinary() const { return _binary; }
  void          FromFile(const char*);
  void          ToFile(const char *sInfo) const;
  
  // miscelaneous functions
  ID3_Field&    operator=( const ID3_Field & );
  bool          InScope(ID3_V2Spec spec) const
  { return _spec_begin <= spec && spec <= _spec_end; }

  ID3_FieldID   GetID() const { return _id; }
  ID3_FieldType GetType() const { return _type; }
  bool          SetEncoding(ID3_TextEnc enc);
  ID3_TextEnc   GetEncoding() const { return _enc; }
  bool          IsEncodable() const { return (_flags & ID3FF_ENCODABLE) > 0; }
  

  void          Render(ID3_Writer&) const;
  bool          Parse(ID3_Reader&);
  bool          HasChanged() const;

private:
  //void          SetSpec(ID3_V2Spec);
  size_t        Set_i(const char*, size_t);
  size_t        Set_i(const unicode_t*, size_t);
  size_t        Add_i(const char*, size_t);
  size_t        Add_i(const unicode_t*, size_t);

private:
  // To prevent public instantiation, the constructor is made private
  ID3_FieldImpl();
  ID3_FieldImpl(const ID3_FieldDef&);

  const ID3_FieldID   _id;          // the ID of this field
  const ID3_FieldType _type;        // what type is this field or should be
  const ID3_V2Spec    _spec_begin;  // spec end
  const ID3_V2Spec    _spec_end;    // spec begin
  const flags_t       _flags;       // special field flags
  mutable bool        _changed;     // field changed since last parse/render?
  union                             // anonymous union; contents in class scope
  {
    uchar*            _binary;      // for binary strings
    char*             _ascii;       // for ascii strings
    unicode_t*        _unicode;     // for unicode strings
    uint32            _integer;     // for numbers
  };
  const size_t        _fixed_length;// for fixed length fields (0 if not)
  union
  {
    size_t            _bytes;       // the size of the field in bytes
    size_t            _chars;       // the number of characters in the field
  };
  size_t              _num_items;   // the number of items in the text string
  ID3_TextEnc         _enc;         // encoding for text fields
protected:
  void RenderInteger(ID3_Writer&) const;
  void RenderString(ID3_Writer&) const;
  void RenderBinary(ID3_Writer&) const;
  
  bool ParseInteger(ID3_Reader&);
  bool ParseASCIIString(ID3_Reader&);
  bool ParseUnicodeString(ID3_Reader&);
  bool ParseBinary(ID3_Reader&);
  
};

// Ack! Not for public use
ID3_FrameDef *ID3_FindFrameDef(ID3_FrameID id);
ID3_FrameID   ID3_FindFrameID(const char *id);

#endif /* _ID3LIB_FIELD_H_ */
