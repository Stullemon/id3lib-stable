// $Id: globals.h,v 1.1 1999/11/25 18:13:50 scott Exp $
//
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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/**
 * Shorthand type definitions
 */
typedef       unsigned char   uchar;
typedef short   signed int    ssint;
typedef short unsigned int    suint;
typedef long    signed int    lsint;
typedef long  unsigned int    luint;
typedef long           double ldoub;
typedef long  unsigned int *  bitset;

/**
 * Enumeration of the types of text encodings: ascii or unicode
 */
enum ID3_TextEnc
{
  ID3TE_ASCII = 0,
  ID3TE_UNICODE
};


/**
 * Enumeration of the different types of fields in a frame.
 */
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

/**
 * Enumeration of the different types of frames recognized by id3lib
 */
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

#endif /* __GLOBALS_H__ */
