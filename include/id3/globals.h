/* $Id: globals.h,v 1.9 2000/04/20 03:38:45 eldamitri Exp $

 * id3lib: a C++ library for creating and manipulating id3v1/v2 tags Copyright
 * 1999, 2000 Scott Thomas Haug

 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 * The id3lib authors encourage improvements and optimisations to be sent to
 * the id3lib coordinator.  Please see the README file for details on where to
 * send such submissions.  See the AUTHORS file for a list of people who have
 * contributed to id3lib.  See the ChangeLog file for a list of changes to
 * id3lib.  These files are distributed with id3lib at
 * http://download.sourceforge.net/id3lib/ 
 */

/** This file defines common macros, types, constants, and enums used
 ** throughout id3lib.
 **/

#ifndef __ID3LIB_GLOBALS_H__
#define __ID3LIB_GLOBALS_H__

#include "sized_types.h"

/* id3lib version.
 * we prefix variable declarations so they can
 * properly get exported in windows dlls.
 * (borrowed from glib.h http://www.gtk.org)
 */
#ifdef NATIVE_WIN32
#  ifdef ID3LIB_COMPILATION
#    define ID3_EXTERN __declspec(dllexport)
#  else /* !ID3LIB_COMPILATION */
#    define ID3_EXTERN extern __declspec(dllimport)
#  endif /* !ID3LIB_COMPILATION */
#else /* !NATIVE_WIN32 */
#  define ID3_EXTERN extern
#endif /* !NATIVE_WIN32 */

#ifndef __cplusplus

typedef int bool;
#define false (0)
#define true (!false)

#endif /* __cplusplus */

ID3_EXTERN const char * const ID3LIB_NAME;
ID3_EXTERN const char * const ID3LIB_RELEASE;
ID3_EXTERN const char * const ID3LIB_FULL_NAME;
ID3_EXTERN const int          ID3LIB_MAJOR_VERSION;
ID3_EXTERN const int          ID3LIB_MINOR_VERSION;
ID3_EXTERN const int          ID3LIB_PATCH_VERSION;
ID3_EXTERN const int          ID3LIB_INTERFACE_AGE;
ID3_EXTERN const int          ID3LIB_BINARY_AGE;

typedef       unsigned char   uchar;
typedef short   signed int    ssint;
typedef short unsigned int    suint;
typedef long    signed int    lsint;
typedef long  unsigned int    luint;
typedef long           double ldoub;
typedef long  unsigned int *  bitset;
typedef uint16                unicode_t;
const unicode_t NULL_UNICODE = (unicode_t) '\0';

/**
 ** Enumeration of the types of text encodings: ascii or unicode
 **/
enum ID3_TextEnc
{
  ID3TE_ASCII = 0,
  ID3TE_UNICODE
};

/** Enumeration of the various id3 specifications
 **/
enum ID3_SpecVersion
{
  ID3V1_0 = 0,
  ID3V1_1,
  ID3V2_2_0,
  ID3V2_2_1,
  ID3V2_3_0,
  ID3_LASTVERSION
};

const ID3_SpecVersion ID3_LATEST_SPEC = ID3V2_3_0;

/**
 ** Enumeration of the different types of fields in a frame.
 **/
enum ID3_FieldID
{
  ID3FN_NOFIELD = 0,    /**< No field */
  ID3FN_TEXTENC,        /**< Text encoding (unicode or ASCII) */
  ID3FN_TEXT,           /**< Text field */
  ID3FN_URL,            /**< A URL */
  ID3FN_DATA,           /**< Data field */
  ID3FN_DESCRIPTION,    /**< Description field */
  ID3FN_OWNER,          /**< Owner field */
  ID3FN_EMAIL,          /**< Email field */
  ID3FN_RATING,         /**< Rating field */
  ID3FN_FILENAME,       /**< Filename field */
  ID3FN_LANGUAGE,       /**< Language field */
  ID3FN_PICTURETYPE,    /**< Picture type field */
  ID3FN_IMAGEFORMAT,    /**< Image format field */
  ID3FN_MIMETYPE,       /**< Mimetype field */
  ID3FN_COUNTER,        /**< Counter field */
  ID3FN_ID,             /**< Identifier/Symbol field */
  ID3FN_VOLUMEADJ,      /**< Volume adjustment field */
  ID3FN_NUMBITS,        /**< Number of bits field */
  ID3FN_VOLCHGRIGHT,    /**< Volume chage on the right channel */
  ID3FN_VOLCHGLEFT,     /**< Volume chage on the left channel */
  ID3FN_PEAKVOLRIGHT,   /**< Peak volume on the right channel */
  ID3FN_PEAKVOLLEFT,    /**< Peak volume on the left channel */
  ID3FN_LASTFIELDID     /**< Last field placeholder */
};

/**
 ** Enumeration of the different types of frames recognized by id3lib
 **/
enum ID3_FrameID
{
  /* ???? */ ID3FID_NOFRAME = 0,       /**< No known frame */
  /* AENC */ ID3FID_AUDIOCRYPTO,       /**< Audio encryption */
  /* APIC */ ID3FID_PICTURE,           /**< Attached picture */
  /* COMM */ ID3FID_COMMENT,           /**< Comments */
  /* COMR */ ID3FID_COMMERCIAL,        /**< Commercial frame */
  /* ENCR */ ID3FID_CRYPTOREG,         /**< Encryption method registration */
  /* EQUA */ ID3FID_EQUALIZATION,      /**< Equalization */
  /* ETCO */ ID3FID_EVENTTIMING,       /**< Event timing codes */
  /* GEOB */ ID3FID_GENERALOBJECT,     /**< General encapsulated object */
  /* GRID */ ID3FID_GROUPINGREG,       /**< Group identification registration */
  /* IPLS */ ID3FID_INVOLVEDPEOPLE,    /**< Involved people list */
  /* LINK */ ID3FID_LINKEDINFO,        /**< Linked information */
  /* MCDI */ ID3FID_CDID,              /**< Music CD identifier */
  /* MLLT */ ID3FID_MPEGLOOKUP,        /**< MPEG location lookup table */
  /* OWNE */ ID3FID_OWNERSHIP,         /**< Ownership frame */
  /* PRIV */ ID3FID_PRIVATE,           /**< Private frame */
  /* PCNT */ ID3FID_PLAYCOUNTER,       /**< Play counter */
  /* POPM */ ID3FID_POPULARIMETER,     /**< Popularimeter */
  /* POSS */ ID3FID_POSITIONSYNC,      /**< Position synchronisation frame */
  /* RBUF */ ID3FID_BUFFERSIZE,        /**< Recommended buffer size */
  /* RVAD */ ID3FID_VOLUMEADJ,         /**< Relative volume adjustment */
  /* RVRB */ ID3FID_REVERB,            /**< Reverb */
  /* SYLT */ ID3FID_SYNCEDLYRICS,      /**< Synchronized lyric/text */
  /* SYTC */ ID3FID_SYNCEDTEMPO,       /**< Synchronized tempo codes */
  /* TALB */ ID3FID_ALBUM,             /**< Album/Movie/Show title */
  /* TBPM */ ID3FID_BPM,               /**< BPM (beats per minute) */
  /* TCOM */ ID3FID_COMPOSER,          /**< Composer */
  /* TCON */ ID3FID_CONTENTTYPE,       /**< Content type */
  /* TCOP */ ID3FID_COPYRIGHT,         /**< Copyright message */
  /* TDAT */ ID3FID_DATE,              /**< Date */
  /* TDLY */ ID3FID_PLAYLISTDELAY,     /**< Playlist delay */
  /* TENC */ ID3FID_ENCODEDBY,         /**< Encoded by */
  /* TEXT */ ID3FID_LYRICIST,          /**< Lyricist/Text writer */
  /* TFLT */ ID3FID_FILETYPE,          /**< File type */
  /* TIME */ ID3FID_TIME,              /**< Time */
  /* TIT1 */ ID3FID_CONTENTGROUP,      /**< Content group description */
  /* TIT2 */ ID3FID_TITLE,             /**< Title/songname/content description */
  /* TIT3 */ ID3FID_SUBTITLE,          /**< Subtitle/Description refinement */
  /* TKEY */ ID3FID_INITIALKEY,        /**< Initial key */
  /* TLAN */ ID3FID_LANGUAGE,          /**< Language(s) */
  /* TLEN */ ID3FID_SONGLEN,           /**< Length */
  /* TMED */ ID3FID_MEDIATYPE,         /**< Media type */
  /* TOAL */ ID3FID_ORIGALBUM,         /**< Original album/movie/show title */
  /* TOFN */ ID3FID_ORIGFILENAME,      /**< Original filename */
  /* TOLY */ ID3FID_ORIGLYRICIST,      /**< Original lyricist(s)/text writer(s) */
  /* TOPE */ ID3FID_ORIGARTIST,        /**< Original artist(s)/performer(s) */
  /* TORY */ ID3FID_ORIGYEAR,          /**< Original release year */
  /* TOWN */ ID3FID_FILEOWNER,         /**< File owner/licensee */
  /* TPE1 */ ID3FID_LEADARTIST,        /**< Lead performer(s)/Soloist(s) */
  /* TPE2 */ ID3FID_BAND,              /**< Band/orchestra/accompaniment */
  /* TPE3 */ ID3FID_CONDUCTOR,         /**< Conductor/performer refinement */
  /* TPE4 */ ID3FID_MIXARTIST,         /**< Interpreted, remixed, or otherwise modified by */
  /* TPOS */ ID3FID_PARTINSET,         /**< Part of a set */
  /* TPUB */ ID3FID_PUBLISHER,         /**< Publisher */
  /* TRCK */ ID3FID_TRACKNUM,          /**< Track number/Position in set */
  /* TRDA */ ID3FID_RECORDINGDATES,    /**< Recording dates */
  /* TRSN */ ID3FID_NETRADIOSTATION,   /**< Internet radio station name */
  /* TRSO */ ID3FID_NETRADIOOWNER,     /**< Internet radio station owner */
  /* TSIZ */ ID3FID_SIZE,              /**< Size */
  /* TSRC */ ID3FID_ISRC,              /**< ISRC (international standard recording code) */
  /* TSSE */ ID3FID_ENCODERSETTINGS,   /**< Software/Hardware and settings used for encoding */
  /* TXXX */ ID3FID_USERTEXT,          /**< User defined text information */
  /* TYER */ ID3FID_YEAR,              /**< Year */
  /* UFID */ ID3FID_UNIQUEFILEID,      /**< Unique file identifier */
  /* USER */ ID3FID_TERMSOFUSE,        /**< Terms of use */
  /* USLT */ ID3FID_UNSYNCEDLYRICS,    /**< Unsynchronized lyric/text transcription */
  /* WCOM */ ID3FID_WWWCOMMERCIALINFO, /**< Commercial information */
  /* WCOP */ ID3FID_WWWCOPYRIGHT,      /**< Copyright/Legal infromation */
  /* WOAF */ ID3FID_WWWAUDIOFILE,      /**< Official audio file webpage */
  /* WOAR */ ID3FID_WWWARTIST,         /**< Official artist/performer webpage */
  /* WOAS */ ID3FID_WWWAUDIOSOURCE,    /**< Official audio source webpage */
  /* WORS */ ID3FID_WWWRADIOPAGE,      /**< Official internet radio station homepage */
  /* WPAY */ ID3FID_WWWPAYMENT,        /**< Payment */
  /* WPUB */ ID3FID_WWWPUBLISHER,      /**< Official publisher webpage */
  /* WXXX */ ID3FID_WWWUSER,           /**< User defined URL link */
  /*      */ ID3FID_METACRYPTO         /**< Encrypted meta frame (id3v2.2.x) */
};

#define BS_SIZE (sizeof(luint)*8)
#define BS_SET(v,x)   ((v)[(x) / BS_SIZE] |=  (1 << ((x) % BS_SIZE)))
#define BS_CLEAR(v,x) ((v)[(x) / BS_SIZE] &= ~(1 << ((x) % BS_SIZE)))
#define BS_ISSET(v,x) ((v)[(x) / BS_SIZE] &   (1 << ((x) % BS_SIZE)))

#define MASK(bits) ((1 << (bits)) - 1)
#define MASK1 MASK(1)
#define MASK2 MASK(2)
#define MASK3 MASK(3)
#define MASK4 MASK(4)
#define MASK5 MASK(5)
#define MASK6 MASK(6)
#define MASK7 MASK(7)
#define MASK8 MASK(8)

/*
 * The following is borrowed from glib.h (http://www.gtk.org)
 */
#ifdef NATIVE_WIN32

/* On native Win32, directory separator is the backslash, and search path
 * separator is the semicolon.
 */
#  define ID3_DIR_SEPARATOR '\\'
#  define ID3_DIR_SEPARATOR_S "\\"
#  define ID3_SEARCHPATH_SEPARATOR ';'
#  define ID3_SEARCHPATH_SEPARATOR_S ";"

#else  /* !NATIVE_WIN32 */

#  ifndef __EMX__
/* Unix */

#    define ID3_DIR_SEPARATOR '/'
#    define ID3_DIR_SEPARATOR_S "/"
#    define ID3_SEARCHPATH_SEPARATOR ':'
#    define ID3_SEARCHPATH_SEPARATOR_S ":"

#  else
/* EMX/OS2 */

#    define ID3_DIR_SEPARATOR '/'
#    define ID3_DIR_SEPARATOR_S "/"
#    define ID3_SEARCHPATH_SEPARATOR ';'
#    define ID3_SEARCHPATH_SEPARATOR_S ";"

#  endif

#endif /* !NATIVE_WIN32 */

#ifndef NULL
#define NULL ((void*) 0)
#endif

#undef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
  
#undef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
  
#undef ABS
#define ABS(a)   (((a) < 0) ? -(a) : (a))

#undef CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#endif /* __ID3LIB_GLOBALS_H__ */
