// $Id: frame.h,v 1.33 2000/10/09 01:53:37 eldamitri Exp $

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

#ifndef _ID3LIB_FRAME_H_
#define _ID3LIB_FRAME_H_

#include "spec.h"

class ID3_Field;
class ID3_FrameImpl;
class ID3_Reader;
class ID3_Writer;

class ID3_Frame : public ID3_Speccable
{
  ID3_FrameImpl* _impl;
public:
  ID3_Frame(ID3_FrameID id = ID3FID_NOFRAME);
  ID3_Frame(const ID3_Frame&);

  /// Destructor.
  virtual ~ID3_Frame();
  
  void        Clear();

  bool        SetID(ID3_FrameID id);
  ID3_FrameID GetID() const;
  
  ID3_Field*  GetField(ID3_FieldID name) const;

  size_t      NumFields() const;
  ID3_Field*  GetFieldNum(index_t) const;
  
  const char* GetDescription() const;
  static const char* GetDescription(ID3_FrameID);

  const char* GetTextID() const;

  ID3_Frame&  operator=(const ID3_Frame &);
  bool        HasChanged() const;
  bool        Parse(ID3_Reader&);
  void        Render(ID3_Writer&) const;
  size_t      Size();
  bool        Contains(ID3_FieldID fld) const;
  bool        SetSpec(ID3_V2Spec);
  ID3_V2Spec  GetSpec() const;

  /** Sets the compression flag within the frame.  When the compression flag is
   ** is set, compression will be attempted.  However, the frame might not
   ** actually be compressed after it is rendered if the "compressed" data is
   ** no smaller than the "uncompressed" data.
   **/
  bool        SetCompression(bool b);
  /** Returns whether or not the compression flag is set.  After parsing a tag,
   ** this will indicate whether or not the frame was compressed.  After
   ** rendering a tag, however, it does not actually indicate if the frame is
   ** compressed rendering.  It only indicates whether or not compression was
   ** attempted.  A frame will not be compressed, even whent the compression
   ** flag is set, if the "compressed" data is no smaller than the
   ** "uncompressed" data.
   **/
  bool        GetCompression() const;
  size_t      GetDataSize() const;

  bool        SetEncryptionID(uchar id);
  uchar       GetEncryptionID() const;
  
  bool        SetGroupingID(uchar id);
  uchar       GetGroupingID() const;

  // Deprecated
  ID3_Field&  Field(ID3_FieldID name) const;
};

#endif /* _ID3LIB_FRAME_H_ */
