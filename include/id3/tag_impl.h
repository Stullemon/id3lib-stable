// -*- C++ -*-
// $Id: tag_impl.h,v 1.2 2000/10/09 01:56:48 eldamitri Exp $

// id3lib: a software library for creating and manipulating id3v1/v2 tags
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

#ifndef _ID3LIB_TAG_IMPL_H_
#define _ID3LIB_TAG_IMPL_H_

#include <stdio.h>
#include "tag.h"
#include "header_tag.h"
#include "frame.h"
#include "field.h"
#include "spec.h"

class ID3_Reader;
class ID3_Writer;

struct ID3_Elem
{
  virtual ~ID3_Elem() { if (pFrame) { delete pFrame; } }
  ID3_Elem  *pNext;
  ID3_Frame *pFrame;
};

namespace dami
{
  namespace id3
  {
    namespace v1
    {
      bool parse(ID3_TagImpl&, ID3_Reader&);
      void render(ID3_Writer&, const ID3_TagImpl&);
    };
    namespace v2
    {
      bool parse(ID3_TagImpl& tag, ID3_Reader& rdr);
      void render(ID3_Writer& writer, const ID3_TagImpl& tag);
    };
  };
  namespace lyr3
  {
    namespace v1
    {
      bool parse(ID3_TagImpl&, ID3_Reader&);
    };
    namespace v2
    {
      bool parse(ID3_TagImpl&, ID3_Reader&);
    };
  };
  namespace mm
  {
    bool parse(ID3_TagImpl&, ID3_Reader&);
  };
};

class ID3_TagImpl : public ID3_Speccable
{
public:
  ID3_TagImpl(const char *name = NULL);
  ID3_TagImpl(const ID3_Tag &tag);
  virtual ~ID3_TagImpl();
  
  void       Clear();
  bool       HasChanged() const;
  bool       SetChanged(bool b) { _changed = b; }
  size_t     Size() const;
  
  bool       SetUnsync(bool);
  bool       SetExtended(bool);
  bool       SetExperimental(bool);
  bool       SetPadding(bool);

  bool       GetUnsync() const;
  bool       GetExtended() const;
  bool       GetExperimental() const;
  
  void       AddFrame(const ID3_Frame&);
  void       AddFrame(const ID3_Frame*);
  void       AttachFrame(ID3_Frame*);
  ID3_Frame* RemoveFrame(const ID3_Frame *);
  
  size_t     Link(const char *fileInfo, flags_t = (flags_t) ID3TT_ALL);
  flags_t    Update(flags_t = (flags_t) ID3TT_ALL);
  flags_t    Strip(flags_t = (flags_t) ID3TT_ALL);
  
  size_t     GetPrependedBytes() const { return _prepended_bytes; }
  size_t     GetAppendedBytes() const { return _appended_bytes; }
  size_t     GetFileSize() const { return _file_size; }
  const char* GetFileName() const { return _file_name; }
  
  /// Finds frame with given frame id
  ID3_Frame* Find(ID3_FrameID id) const;
  
  /// Finds frame with given frame id, fld id, and integer data
  ID3_Frame* Find(ID3_FrameID id, ID3_FieldID fld, uint32 data) const;
  
  /// Finds frame with given frame id, fld id, and ascii data
  ID3_Frame* Find(ID3_FrameID id, ID3_FieldID fld, const char*) const;
  
  /// Finds frame with given frame id, fld id, and unicode data
  ID3_Frame* Find(ID3_FrameID id, ID3_FieldID fld, const unicode_t*) const;
  
  /** Returns the number of frames present in the tag object.
   ** 
   ** This includes only those frames that id3lib recognises.  This is used as
   ** the upper bound on calls to the GetFrame() and operator[]() methods.
   ** 
   ** \return The number of frames present in the tag object.
   **/
  size_t     NumFrames() const { return _num_frames; }
  ID3_Frame* GetFrameNum(index_t) const;
  ID3_Frame* operator[](index_t) const;
  ID3_TagImpl&   operator=( const ID3_Tag & );
  
  bool       HasTagType(uint16 tt) const { return _file_tags.test(tt); }
  ID3_V2Spec GetSpec() const;
  bool       SetSpec(ID3_V2Spec);
  
  static size_t IsV2Tag(ID3_Reader&);
  
  /* Deprecated! */
  void       AddNewFrame(ID3_Frame* f) { this->AttachFrame(f); }
  size_t     Link(const char *fileInfo, bool parseID3v1, bool parseLyrics3);
  void       SetCompression(bool) { ; }
  void       AddFrames(const ID3_Frame *, size_t);
  bool       HasLyrics() const { return this->HasTagType(ID3TT_LYRICS); }
  bool       HasV2Tag()  const { return this->HasTagType(ID3TT_ID3V2); }
  bool       HasV1Tag()  const { return this->HasTagType(ID3TT_ID3V1); }
  size_t     PaddingSize(size_t) const;
  
protected:
  ID3_Elem*  Find(const ID3_Frame *) const;
  
  void       RenderExtHeader(uchar *);

  void       ParseFile();
  
private:
  ID3_TagHeader _hdr;          // information relevant to the tag header
  bool       _is_padded;       // add padding to tags?
  
  ID3_Elem*  _frames;          // frames attached to the tag
  size_t     _num_frames;      // the current number of frames
  
  mutable ID3_Elem*  _cursor;  // which frame in list are we at
  mutable bool       _changed; // has tag changed since last parse or render?
  
  // file-related member variables
  char*      _file_name;       // name of the file we are linked to
  size_t     _file_size;       // the size of the file (without any tag(s))
  size_t     _prepended_bytes; // number of tag bytes at start of file
  size_t     _appended_bytes;  // number of tag bytes at end of file
  bool       _is_file_writable;// is the associated file (via Link) writable?
  ID3_Flags  _tags_to_parse;   // which tag types should attempt to be parsed
  ID3_Flags  _file_tags;       // which tag types does the file contain
};

size_t     ID3_GetDataSize(const ID3_TagImpl&);

#endif /* _ID3LIB_TAG_IMPL_H_ */
