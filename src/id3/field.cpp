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


#include <string.h>
#include <id3/field.h>

// STH: this was used as a replacement for NULL below to eliminate compiler
// warnings under g++
static luint LU_NULL = (luint) NULL;

static ID3_FieldDef ID3FD_URL[] =
{ 
  {
    ID3FN_URL,                          // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_UserURL[] =
{
  {
    ID3FN_TEXTENC,                      // FIELD NAME        
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DESCRIPTION,                  // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL | ID3FF_ADJUSTENC,       // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_URL,                          // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_Text[] =
{
  {
    ID3FN_TEXTENC,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_TEXT,                         // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_ADJUSTENC,                    // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
  
static ID3_FieldDef ID3FD_UserText[] =
{
  {
    ID3FN_TEXTENC,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DESCRIPTION,                  // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL | ID3FF_ADJUSTENC,       // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_TEXT,                         // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_ADJUSTENC,                    // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
  
static ID3_FieldDef ID3FD_GeneralText[] =
{
  {
    ID3FN_TEXTENC,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_LANGUAGE,                     // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    3, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DESCRIPTION,                  // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL | ID3FF_ADJUSTENC,       // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_TEXT,                         // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_ADJUSTENC,                    // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};

static ID3_FieldDef ID3FD_Picture[] =
{
  {
    ID3FN_TEXTENC,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_IMAGEFORMAT,                  // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    3, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_LOWER,                        // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_MIMETYPE,                     // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 3, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL,                         // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_PICTURETYPE,                  // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DESCRIPTION,                  // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL | ID3FF_ADJUSTENC,       // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DATA,                         // FIELD NAME
    ID3FTY_BINARY,                      // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_GEO[] =
{
  {
    ID3FN_TEXTENC,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_MIMETYPE,                     // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL,                         // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_FILENAME,                     // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL | ID3FF_ADJUSTENC,       // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DESCRIPTION,                  // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL | ID3FF_ADJUSTENC,       // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DATA,                         // FIELD NAME
    ID3FTY_BINARY,                      // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_UFI[] =
{
  {
    ID3FN_OWNER,                        // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL,                         // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DATA,                         // FIELD NAME
    ID3FTY_BINARY,                      // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_PlayCounter[] =
{
  {
    ID3FN_COUNTER,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    4, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_Popularimeter[] =
{
  {
    ID3FN_EMAIL,                        // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL,                         // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_RATING,                       // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_COUNTER,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    4, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_Registration[] =
{
  {
    ID3FN_OWNER,                        // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 3, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL,                         // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_SYMBOL,                       // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 3, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_DATA,                         // FIELD NAME
    ID3FTY_BINARY,                      // FIELD TYPE
    -1, 3, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_InvolvedPeople[] =
{
  {
    ID3FN_TEXTENC,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_TEXT,                         // FIELD NAME
    ID3FTY_ASCIISTRING,                 // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_NULL | ID3FF_NULLDIVIDE | ID3FF_ADJUSTENC, // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static ID3_FieldDef ID3FD_Volume[] =
{
  {
    ID3FN_VOLUMEADJ,                    // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_NUMBITS,                      // FIELD NAME
    ID3FTY_INTEGER,                     // FIELD TYPE
    1, 2, 0,                            // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    LU_NULL,                            // FLAGS
    ID3FN_NOFIELD                       // LINKED FIELD
  },
  {
    ID3FN_VOLCHGRIGHT,                  // FIELD NAME
    ID3FTY_BITFIELD,                    // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_ADJUSTEDBY,                   // FLAGS
    ID3FN_NUMBITS                       // LINKED FIELD
  },
  {
    ID3FN_VOLCHGLEFT,                   // FIELD NAME
    ID3FTY_BITFIELD,                    // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_ADJUSTEDBY,                   // FLAGS
    ID3FN_NUMBITS                       // LINKED FIELD
  },
  {
    ID3FN_PEAKVOLRIGHT,                 // FIELD NAME
    ID3FTY_BITFIELD,                    // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_ADJUSTEDBY,                   // FLAGS
    ID3FN_NUMBITS                       // LINKED FIELD
  },
  {
    ID3FN_PEAKVOLLEFT,                  // FIELD NAME
    ID3FTY_BITFIELD,                    // FIELD TYPE
    -1, 2, 0,                           // FIXED LEN, VERSION, REVISION
    ID3VC_HIGHER,                       // WHAT?
    ID3FF_ADJUSTEDBY,                   // FLAGS
    ID3FN_NUMBITS                       // LINKED FIELD
  },
  { ID3FN_NOFIELD }
};
  
static  ID3_FrameDef ID3_FrameDefs[] =
{
  // FRAMEID,SHORTID,LONGID,PRI,TAGDISCARD,FILEDISCARD,HANDLER,FIELDDEFS
  { ID3FID_ENCODEDBY, "TEN", "TENC", 0, false, true, NULL, ID3FD_Text },
  { ID3FID_ORIGALBUM, "TOT", "TOAL", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_PUBLISHER, "TPB", "TPUB", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_ENCODERSETTINGS, "TSS", "TSSE", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_ORIGFILENAME, "TOF", "TOFN", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_LANGUAGE, "TLA", "TLAN", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_PARTINSET, "TPA", "TPOS", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_DATE, "TDA", "TDAT", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_TIME, "TIM", "TIME", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_RECORDINGDATES, "TRD", "TRDA", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_MEDIATYPE, "TMT", "TMED", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_FILETYPE, "TFT", "TFLT", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_NETRADIOSTATION, "TRN", "TRSN", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_NETRADIOOWNER, "TRO", "TRDO", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_LYRICIST, "TXT", "TEXT", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_ORIGARTIST, "TOA", "TOPE", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_ORIGLYRICIST, "TOL", "TOLY", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_CONTENTGROUP, "TT1", "TIT1", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_TITLE, "TT2", "TIT2", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_SUBTITLE, "TT3", "TIT3", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_LEADARTIST, "TP1", "TPE1", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_BAND, "TP2", "TPE2", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_CONDUCTOR, "TP3", "TPE3", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_MIXARTIST, "TP4", "TPE4", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_ALBUM, "TAL", "TALB", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_YEAR, "TYE", "TYER", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_COMPOSER, "TCM", "TCOM", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_COPYRIGHT, "TCR", "TCOP", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_CONTENTTYPE, "TCO", "TCON", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_TRACKNUM, "TRK", "TRCK", 0, false, false, NULL, ID3FD_Text },
  { ID3FID_USERTEXT, "TXX", "TXXX", 0, false, false, NULL, ID3FD_UserText },
  { ID3FID_COMMENT, "COM", "COMM", 0, false, false, NULL, ID3FD_GeneralText },
  { ID3FID_UNSYNCEDLYRICS, "ULT", "USLT", 0, false, false, NULL, ID3FD_GeneralText },
  // URL Frames
  { ID3FID_WWWAUDIOFILE, "WAF", "WOAF", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWARTIST, "WAR", "WOAR", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWAUDIOSOURCE, "WAS", "WOAS", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWCOMMERCIALINFO, "WCM", "WCOM", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWCOPYRIGHT, "WCP", "WCOP", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWPUBLISHER, "WPB", "WPUB", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWPAYMENT, "WPY", "WPAY", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWRADIOPAGE, "WRA", "WORS", 0, false, false, NULL, ID3FD_URL },
  { ID3FID_WWWUSER, "WXX", "WXXX", 0, false, false, NULL, ID3FD_UserURL },
  // misc frames
  { ID3FID_INVOLVEDPEOPLE, "IPL", "IPLS", 0, false, false, NULL, ID3FD_InvolvedPeople },
  { ID3FID_PICTURE, "PIC", "APIC", 0, false, false, NULL, ID3FD_Picture },
  { ID3FID_GENERALOBJECT, "GEO", "GEOB", 0, false, false, NULL, ID3FD_GEO },
  { ID3FID_UNIQUEFILEID, "UFI", "UFID", 0, false, false, NULL, ID3FD_UFI },
  { ID3FID_PLAYCOUNTER, "CNT", "PCNT", 0, false, false, NULL, ID3FD_PlayCounter },
  { ID3FID_POPULARIMETER, "POP", "POPM", 0, false, false, NULL, ID3FD_Popularimeter },
  { ID3FID_CRYPTOREG, "   ", "ENCR", 0, false, false, NULL, ID3FD_Registration },
  { ID3FID_GROUPINGREG, "   ", "GRID", 0, false, false, NULL, ID3FD_Registration },
  { ID3FID_NOFRAME  }
};
  
ID3_Field::ID3_Field(void)
{
  name  = ID3FN_NOFIELD;
  type  = ID3FTY_INTEGER;
  data  = 0;
  size  = 0;
  flags = 0;
  
  Clear();
}

ID3_Field::~ID3_Field(void)
{
  Clear();
}

void
ID3_Field::Clear(void)
{
  if ( data && size && type != ID3FTY_INTEGER )
    delete[] data;
    
  type       = ID3FTY_INTEGER;
  data       = 0;
  size       = sizeof (luint);
  hasChanged = true;
  
  return ;
}

void
ID3_Field::SetVersion(uchar ver, uchar rev)
{
  // If the version or revision has changed, set the hasChanged flag
  hasChanged = (hasChanged || version != ver || rev != rev);

  version = ver;
  revision = rev;
  
  return ;
}

bool
ID3_Field::HasChanged(void)
{
  return hasChanged;
}

luint
ID3_Field::Size(void)
{
  return BinSize(false);
}

luint
ID3_Field::BinSize(bool withExtras)
{
  luint bytes   = 0;
  
  if ((control == ID3VC_HIGHER &&
       version >= ioVersion && revision >= ioRevision) ||
      (version <= ioVersion && revision <= ioRevision))
  {
    bytes = size;
    
    if (withExtras)
    {
      if (! data && size)
      {
        bytes = (flags & ID3FF_NULL) ? 2 : 0;
      }
      
      // if we are a Unicode string, add 2 bytes for the BOM (but only if there
      // is a string to render - regardless of NULL)
      if (type == ID3FTY_UNICODESTRING && data && size)
        bytes += 2;
        
      // if we are an ASCII string, divide by sizeof ( wchar_t ) because
      // internally we store the string as Unicode, so the ASCII version will
      // only be half as long
      if (type == ID3FTY_ASCIISTRING)
        bytes /= sizeof (wchar_t);
    }
    else
    {
      // because it seems that the application called us via ID3_Field::Size()
      // we are going to return the number of characters, not bytes, so if the
      // string is Unicode, we will half the 'bytes' variable because Unicode
      // strings have twice as many bytes as they do characters
      if (type == ID3FTY_UNICODESTRING)
        bytes /= sizeof (wchar_t);
    }
    
    // check to see if we are within the legal limit for this field -1 means
    // arbitrary length field
    if (fixedLength != -1)
      bytes = fixedLength;
  }
  
  return bytes;
}

luint
ID3_Field::Parse(uchar *buffer, luint posn, luint buffSize)
{
  luint bytesUsed       = 0;

  if ((control == ID3VC_HIGHER &&
       version >= ioVersion && revision >= ioRevision) ||
      (version <= ioVersion && revision <= ioRevision))
  {
    switch (type)
    {
      case ID3FTY_INTEGER:
        bytesUsed = ParseInteger(buffer, posn, buffSize);
        break;
        
      case ID3FTY_BINARY:
        bytesUsed = ParseBinary( buffer, posn, buffSize);
        break;
        
      case ID3FTY_ASCIISTRING:
        bytesUsed = ParseASCIIString(buffer, posn, buffSize);
        break;
        
      case ID3FTY_UNICODESTRING:
        bytesUsed = ParseUnicodeString(buffer, posn, buffSize);
        break;
        
      default:
        ID3_THROW(ID3E_UnknownFieldType);
        break;
    }
  }
  
  return bytesUsed;
}

ID3_FrameDef *
ID3_FindFrameDef(ID3_FrameID id)
{
  ID3_FrameDef  *info   = NULL;

  for (luint cur = 0; ID3_FrameDefs[cur].id != ID3FID_NOFRAME; cur++)
  {
    if (ID3_FrameDefs[cur].id == id )
    {
      info = &ID3_FrameDefs[cur];
      break;
    }
  }
    
  return info;
}

ID3_FrameID
ID3_FindFrameID(char *id)
{
  ID3_FrameID fid = ID3FID_NOFRAME;
  
  for (luint cur = 0; ID3_FrameDefs[cur].id != ID3FID_NOFRAME; cur++)
  {
    if (((strcmp(ID3_FrameDefs[cur].shortTextID, id) == 0) &&
         strlen(id) == 3) ||
        ((strcmp(ID3_FrameDefs[cur].longTextID,  id) == 0) &&
         strlen(id) == 4))
    {
      fid = ID3_FrameDefs[cur].id;
      break;
    }
  }
  
  return fid;
}

luint
ID3_Field::Render(uchar *buffer)
{
  luint bytesUsed       = 0;
  
  // if ( doIt )
  if ((control == ID3VC_HIGHER &&
       version >= ioVersion && revision >= ioRevision) ||
      (version <= ioVersion && revision <= ioRevision))
    switch ( type ) 
    {
      case ID3FTY_INTEGER:
        bytesUsed = RenderInteger ( buffer );
        break;
        
      case ID3FTY_BINARY:
        bytesUsed = RenderBinary ( buffer );
        break;
        
      case ID3FTY_ASCIISTRING:
        bytesUsed = RenderASCIIString ( buffer );
        break;
        
      case ID3FTY_UNICODESTRING:
        bytesUsed = RenderUnicodeString ( buffer );
        break;
        
      default:
        ID3_THROW ( ID3E_UnknownFieldType );
        break;
    }
    
  return bytesUsed;
}
