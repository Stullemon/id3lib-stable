// $Id: field.h,v 1.6 1999/11/15 20:15:36 scott Exp $

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


#ifndef ID3LIB_FIELD_H
#define ID3LIB_FIELD_H


#include <wchar.h>
#include <id3/types.h>
#include <id3/error.h>


// field flags
const luint ID3FF_NONE       = 0;
const luint ID3FF_NULL       = 1 << 0;
const luint ID3FF_NULLDIVIDE = 1 << 1;
const luint ID3FF_ADJUSTENC  = 1 << 2;
const luint ID3FF_ADJUSTEDBY = 1 << 3;

enum ID3_TextEnc
{
  ID3TE_ASCII   = 0,
  ID3TE_UNICODE
};

enum ID3_FieldType
{
  ID3FTY_INTEGER        = 0,
  ID3FTY_BITFIELD,
  ID3FTY_BINARY,
  ID3FTY_ASCIISTRING,
  ID3FTY_UNICODESTRING
};

enum ID3_FieldID
{
  ID3FN_NOFIELD = 0,
  ID3FN_TEXTENC,
  ID3FN_TEXT,
  ID3FN_URL,
  ID3FN_DATA,
  ID3FN_DESCRIPTION,
  ID3FN_OWNER,
  ID3FN_EMAIL,
  ID3FN_RATING,
  ID3FN_FILENAME,
  ID3FN_LANGUAGE,
  ID3FN_PICTURETYPE,
  ID3FN_IMAGEFORMAT,
  ID3FN_MIMETYPE,
  ID3FN_COUNTER,
  ID3FN_SYMBOL,
  ID3FN_VOLUMEADJ,
  ID3FN_NUMBITS,
  ID3FN_VOLCHGRIGHT,
  ID3FN_VOLCHGLEFT,
  ID3FN_PEAKVOLRIGHT,
  ID3FN_PEAKVOLLEFT,
  
  ID3FN_LASTFIELDID
};

enum ID3_FrameID
{
  ID3FID_NOFRAME        = 0,
  ID3FID_ORIGALBUM,
  ID3FID_PUBLISHER,
  ID3FID_ENCODEDBY,
  ID3FID_ENCODERSETTINGS,
  ID3FID_ORIGFILENAME,
  ID3FID_LANGUAGE,
  ID3FID_PARTINSET,
  ID3FID_DATE,
  ID3FID_TIME,
  ID3FID_RECORDINGDATES,
  ID3FID_MEDIATYPE,
  ID3FID_FILETYPE,
  ID3FID_NETRADIOSTATION,
  ID3FID_NETRADIOOWNER,
  ID3FID_LYRICIST,
  ID3FID_ORIGARTIST,
  ID3FID_ORIGLYRICIST,
  ID3FID_SUBTITLE,
  ID3FID_MIXARTIST,
  ID3FID_USERTEXT,
  ID3FID_CONTENTGROUP,
  ID3FID_TITLE,
  ID3FID_LEADARTIST,
  ID3FID_BAND,
  ID3FID_ALBUM,
  ID3FID_YEAR,
  ID3FID_CONDUCTOR,
  ID3FID_COMPOSER,
  ID3FID_COPYRIGHT,
  ID3FID_CONTENTTYPE,
  ID3FID_TRACKNUM,
  ID3FID_COMMENT,
  ID3FID_WWWAUDIOFILE,
  ID3FID_WWWARTIST,
  ID3FID_WWWAUDIOSOURCE,
  ID3FID_WWWCOMMERCIALINFO,
  ID3FID_WWWCOPYRIGHT,
  ID3FID_WWWPUBLISHER,
  ID3FID_WWWPAYMENT,
  ID3FID_WWWRADIOPAGE,
  ID3FID_WWWUSER,
  ID3FID_INVOLVEDPEOPLE,
  ID3FID_UNSYNCEDLYRICS,
  ID3FID_PICTURE,
  ID3FID_GENERALOBJECT,
  ID3FID_UNIQUEFILEID,
  ID3FID_PLAYCOUNTER,
  ID3FID_POPULARIMETER,
  ID3FID_GROUPINGREG,
  ID3FID_CRYPTOREG
};

enum ID3_VerCtl
{
  ID3VC_HIGHER  = 0,
  ID3VC_LOWER
};

struct ID3_FieldDef
{
  ID3_FieldID   eID;
  ID3_FieldType eType;
  lsint         lFixedLength;
  uchar         ucVersion;
  uchar         ucRevision;
  ID3_VerCtl    eControl;
  luint         ulFlags;
  ID3_FieldID   eLinkedField;
};

class ID3_Frame;
class ID3_Tag;

struct ID3_FrameDef
{
  ID3_FrameID   eID;
  char         *sShortTextID;
  char         *sLongTextID;
  lsint         lPriority;    // currentyl unused
  bool          bTagDiscard;
  bool          bFileDiscard;
  bool  (*parseHandler) (ID3_Frame *frame);
  ID3_FieldDef *aeFieldDefs;
};

class ID3_Field
{
  friend ID3_Frame;
  friend ID3_Tag;
public:
  ID3_Field(void);
  ~ID3_Field(void);
  
  void          Clear(void);
  luint         Size(void);
  luint         GetNumTextItems(void);
  // integer field functions
  ID3_Field    &operator= (luint newData);
  void          Set(luint newData);
  luint         Get(void);
  // Unicode string field functions
  ID3_Field    &operator= (wchar_t *string);
  void          Set(wchar_t *string);
  luint         Get(wchar_t *buffer, luint maxChars, luint itemNum = 1);
  void          Add(wchar_t *string);
  // ASCII string field functions
  ID3_Field    &operator= (char *string);
  void          Set(char *string);
  luint         Get(char *buffer, luint maxChars, luint itemNum = 1);
  void          Add(char *string );
  // binary field functions
  void          Set(uchar *newData, luint newSize);
  void          Get(uchar *buffer, luint buffLength);
  void          FromFile(char *info);
  void          ToFile(char *info);
  
private:
  luint         BinSize(bool withExtras = true);
  bool          HasChanged(void);
  void          SetVersion(uchar ver, uchar rev);
  luint         Render(uchar *buffer);
  luint         Parse(uchar *buffer, luint posn, luint buffSize);

private:
  ID3_FieldID   __eName;           // the ID of this field
  ID3_FieldType __eType;           // what type is this field or should be
  lsint         __lFixedLength;    // length of field (fixed if positive)
  uchar         __ucIOVersion;     // specific version
  uchar         __ucIORevision;    // specific revision
  ID3_VerCtl    __eControl;        // render if ver/rev is higher, or lower than frame::version, frame::revision?
  luint         __ulFlags;         // special field flags
  uchar         __ucVersion;       // the version being rendered/parsed
  uchar         __ucRevision;      // the revision being rendered/parsed
  bool          __bHasChanged;     // field changed since last parse/render?
  uchar        *__sData;
  luint         __ulSize;
protected:
  luint RenderInteger(uchar *buffer);
  luint RenderASCIIString(uchar *buffer);
  luint RenderUnicodeString(uchar *buffer);
  luint RenderBinary(uchar *buffer);
  
  luint ParseInteger(uchar *buffer, luint posn, luint buffSize);
  luint ParseASCIIString(uchar *buffer, luint posn, luint buffSize);
  luint ParseUnicodeString(uchar *buffer, luint posn, luint buffSize);
  luint ParseBinary(uchar *buffer, luint posn, luint buffSize);
  
};

ID3_FrameDef *ID3_FindFrameDef(ID3_FrameID id);
ID3_FrameID   ID3_FindFrameID(char *id);

#endif

// $Log: field.h,v $
// Revision 1.6  1999/11/15 20:15:36  scott
// Made ID3_Frame and ID3_Tag friends of the ID3_Field class to allow
// access to the private members in ID3_Field.  Made ID3FF field
// flags const luints, rather than #define's.  Made member variable
// names of classes and structs more descriptive.
//
// Revision 1.5  1999/11/04 04:32:11  scott
// Initial revision
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
