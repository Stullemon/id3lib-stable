// $Id: globals.h,v 1.3 1999/11/29 19:26:18 scott Exp $
//
// The authors have released ID3Lib as Public Domain (PD) and claim no
// copyright, patent or other intellectual property protection in this work.
// This means that it may be modified, redistributed and used in commercial
// and non-commercial software and hardware without restrictions.  ID3Lib is
// distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
// express or implied.
// 
// The ID3Lib authors encourage improvements and optimisations to be sent to
// the ID3Lib coordinator, currently Scott Haug (sth2@cs.wustl.edu).  Approved
// submissions may be altered, and will be included and released under these
// terms.

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#if !defined SIZE_SPECIFIC_TYPES
#include <wchar.h>
#endif

/// Shorthand type definition for unsigned char
typedef       unsigned char   uchar;
/// Shorthand type definition for short signed int
typedef short   signed int    ssint;
/// Shorthand type definition for short unsigned int
typedef short unsigned int    suint;
/// Shorthand type definition for long signed int
typedef long    signed int    lsint;
/// Shorthand type definition for long unsigned int
typedef long  unsigned int    luint;
/// Shorthand type definition for long double
typedef long           double ldoub;
/// Shorthand type definition for a bitset
typedef long  unsigned int *  bitset;
/// Type representation of a 2-byte unicode character
#if defined SIZE_SPECIFIC_TYPES
typedef uint16                unicode_t;
const unicode_t NULL_UNICODE = (unicode_t) '\0';
#else
typedef wchar_t               unicode_t;
const unicode_t NULL_UNICODE = L'\0';
#endif
/**
 * Enumeration of the types of text encodings: ascii or unicode
 */
enum ID3_TextEnc
{
  /// ASCII
  ID3TE_ASCII = 0,
  /// Unicode
  ID3TE_UNICODE
};


/**
 * Enumeration of the different types of fields in a frame.
 */
enum ID3_FieldID
{
  /// No field
  ID3FN_NOFIELD = 0,
  /// Text encoding (unicode or ASCII)
  ID3FN_TEXTENC,
  /// Text field
  ID3FN_TEXT,
  /// A URL
  ID3FN_URL,
  /// Data field
  ID3FN_DATA,
  /// Description field
  ID3FN_DESCRIPTION,
  /// Owner field
  ID3FN_OWNER,
  /// Email field
  ID3FN_EMAIL,
  /// Rating field
  ID3FN_RATING,
  /// Filename field
  ID3FN_FILENAME,
  /// Language field
  ID3FN_LANGUAGE,
  /// Picture type field
  ID3FN_PICTURETYPE,
  /// Image format field
  ID3FN_IMAGEFORMAT,
  /// Mimetype field
  ID3FN_MIMETYPE,
  /// Counter field
  ID3FN_COUNTER,
  /// Symbol field
  ID3FN_SYMBOL,
  /// Volume adjustment field
  ID3FN_VOLUMEADJ,
  /// Number of bits field
  ID3FN_NUMBITS,
  /// Volume chage on the right channel
  ID3FN_VOLCHGRIGHT,
  /// Volume chage on the left channel
  ID3FN_VOLCHGLEFT,
  /// Peak volume on the right channel
  ID3FN_PEAKVOLRIGHT,
  /// Peak volume on the left channel
  ID3FN_PEAKVOLLEFT,
  /// Last field placeholder
  ID3FN_LASTFIELDID
};

/**
 * Enumeration of the different types of frames recognized by id3lib
 */
enum ID3_FrameID
{
  /// No known frame
  ID3FID_NOFRAME        = 0,
  /// Original album
  ID3FID_ORIGALBUM,
  /// Publisher
  ID3FID_PUBLISHER,
  /// Encoded by
  ID3FID_ENCODEDBY,
  /// Encoder settings
  ID3FID_ENCODERSETTINGS,
  /// Original filename
  ID3FID_ORIGFILENAME,
  /// Language
  ID3FID_LANGUAGE,
  /// Which part in a set of albums
  ID3FID_PARTINSET,
  /// Date
  ID3FID_DATE,
  /// Time
  ID3FID_TIME,
  /// Recording dates
  ID3FID_RECORDINGDATES,
  /// Media type
  ID3FID_MEDIATYPE,
  /// File type
  ID3FID_FILETYPE,
  /// Net radio station
  ID3FID_NETRADIOSTATION,
  /// Net radio owner
  ID3FID_NETRADIOOWNER,
  /// Lyricist
  ID3FID_LYRICIST,
  /// Original artist
  ID3FID_ORIGARTIST,
  /// Original lyricist
  ID3FID_ORIGLYRICIST,
  /// Subtitle
  ID3FID_SUBTITLE,
  /// Mix artist
  ID3FID_MIXARTIST,
  /// User text
  ID3FID_USERTEXT,
  /// Content group
  ID3FID_CONTENTGROUP,
  /// Title
  ID3FID_TITLE,
  /// Lead artist
  ID3FID_LEADARTIST,
  /// Band
  ID3FID_BAND,
  /// Album
  ID3FID_ALBUM,
  /// Year
  ID3FID_YEAR,
  /// Conductor
  ID3FID_CONDUCTOR,
  /// Composer
  ID3FID_COMPOSER,
  /// Copyright
  ID3FID_COPYRIGHT,
  /// Content-type
  ID3FID_CONTENTTYPE,
  /// Track number
  ID3FID_TRACKNUM,
  /// Comment
  ID3FID_COMMENT,
  /// WWW Audio file
  ID3FID_WWWAUDIOFILE,
  /// WWW artist
  ID3FID_WWWARTIST,
  /// WWW audio source
  ID3FID_WWWAUDIOSOURCE,
  /// WWW commercial info
  ID3FID_WWWCOMMERCIALINFO,
  /// WWW copyright
  ID3FID_WWWCOPYRIGHT,
  /// WWW publisher
  ID3FID_WWWPUBLISHER,
  /// WWW payment
  ID3FID_WWWPAYMENT,
  /// WWW radio page
  ID3FID_WWWRADIOPAGE,
  /// WWW user
  ID3FID_WWWUSER,
  /// Involved people list
  ID3FID_INVOLVEDPEOPLE,
  /// Unsynchronised lyrics
  ID3FID_UNSYNCEDLYRICS,
  /// Picture
  ID3FID_PICTURE,
  /// General object
  ID3FID_GENERALOBJECT,
  /// Unique file id
  ID3FID_UNIQUEFILEID,
  /// Play counter
  ID3FID_PLAYCOUNTER,
  /// Popularimeter
  ID3FID_POPULARIMETER,
  /// Grouping reg
  ID3FID_GROUPINGREG,
  /// Cryptography reg
  ID3FID_CRYPTOREG
};

#endif /* __GLOBALS_H__ */
