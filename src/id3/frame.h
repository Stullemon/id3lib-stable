// $Id: frame.h,v 1.6 1999/11/15 20:18:11 scott Exp $

//  The authors have released ID3Lib as Public Domain (PD) and claim no
//  copyright, patent or other intellectual property protection in this work.
//  This means that it may be modified, redistributed and used in commercial
//  and non-commercial software and hardware without restrictions.  ID3Lib is
//  distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
//  express or implied.
//
//  The ID3Lib authors encourage improvements and optimisations to be sent to
//  the ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
//  submissions may be altered, and will be included and released under these
//  terms.
//
//  Mon Nov 23 18:34:01 1998

#include <id3/types.h>
#include <id3/field.h>
#include <id3/header_frame.h>

class ID3_Tag;

class ID3_Frame
{
  friend ID3_Tag;
public:
  ID3_Frame(ID3_FrameID id = ID3FID_NOFRAME);
  ~ID3_Frame(void);
  
  void        Clear(void);
  void        SetID(ID3_FrameID id);
  ID3_FrameID GetID(void) const;
  ID3_Field  &Field(ID3_FieldID name) const;
  
private:
  char        __sEncryptionID[256]; // encryption method used with this frame
  char        __sGroupingID[256];   // the group to which this frame belongs
  bool        __bCompression;       // should we try to compress?
  bool        __bHasChanged;        // frame changed since last parse/render?
  bitset      __auiFieldBits;       // which fields are present?
  ID3_FrameID __FrameID;            // what frame are we?
  uchar       __ucVersion;          // what version tag?
  uchar       __ucRevision;         // what revision tag?
  luint       __ulNumFields;        // how many fields are in this frame?
  ID3_Field **__apFields;           // an array of field object pointers
protected:
  bool        HasChanged(void ) const;
  void        SetVersion(uchar ver, uchar rev);
  void        Parse(uchar *buffer, luint size);
  void        UpdateStringTypes(void);
  void        UpdateFieldDeps(void);
  luint       Size(void);
  luint       Render(uchar *buffer);
  lsint       FindField(ID3_FieldID name) const;
}
;

// $Log: frame.h,v $
// Revision 1.6  1999/11/15 20:18:11  scott
// Made ID3_Tag a friend of ID3_Frame to allow access to the private
// members in ID3_Frame.  Made variable names more descriptive.
// Added const qualifier to appropriate methods.
//
// Revision 1.5  1999/11/04 04:32:11  scott
// Initial revision
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
