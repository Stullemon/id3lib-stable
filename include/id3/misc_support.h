// $Id: misc_support.h,v 1.4 1999/12/05 05:34:41 scott Exp $
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

#ifndef ID3LIB_MISC_SUPPORT_H
#define ID3LIB_MISC_SUPPORT_H

#include <string.h>
#include "types.h"
#include "tag.h"

// defined in 'id3_misc_support.cpp'
// these are 'convenience functions,' to make using the library easier for the 
// most common of operations
char  *ID3_GetArtist(ID3_Tag *tag);
bool   ID3_AddArtist(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveArtists(ID3_Tag *tag);

char  *ID3_GetAlbum(ID3_Tag *tag);
bool   ID3_AddAlbum(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveAlbums(ID3_Tag *tag);

char  *ID3_GetTitle(ID3_Tag *tag);
bool   ID3_AddTitle(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveTitles(ID3_Tag *tag);

char  *ID3_GetYear(ID3_Tag *tag);
bool   ID3_AddYear(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveYears(ID3_Tag *tag);

char  *ID3_GetComment(ID3_Tag *tag);
bool   ID3_AddComment(ID3_Tag *tag, const char *text,
                      const char *sDescription = "", bool bReplace = false);
size_t ID3_RemoveComments(ID3_Tag *tag, const char *sDescription = NULL);

char  *ID3_GetTrack(ID3_Tag *tag);
luint  ID3_GetTrackNum(ID3_Tag *tag);
bool   ID3_AddTrack(ID3_Tag *tag, uchar ucTrack, uchar ucTotal = 0,
                    bool bReplace = false);
bool   ID3_AddTrack(ID3_Tag *tag, uchar ucTrack, bool bReplace);
size_t ID3_RemoveTracks(ID3_Tag *tag);

char  *ID3_GetGenre(ID3_Tag *tag);
luint  ID3_GetGenreNum(ID3_Tag *tag);
bool   ID3_AddGenre(ID3_Tag *tag, luint ucGenre, bool bReplace = false);
size_t ID3_RemoveGenres(ID3_Tag *tag);

char  *ID3_GetLyrics(ID3_Tag *tag);
bool   ID3_AddLyrics(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveLyrics(ID3_Tag *tag);

void   mbstoucs(unicode_t *unicode, const char *ascii, const luint len);
void   ucstombs(char *ascii, const unicode_t *unicode, const luint len);
size_t ucslen(const unicode_t *unicode);
void   ucscpy(unicode_t *dest, const unicode_t *src);
void   ucsncpy(unicode_t *dest, const unicode_t *src, size_t len);
int    ucscmp(const unicode_t *s1, const unicode_t *s2);
int    ucsncmp(const unicode_t *s1, const unicode_t *s2, size_t len);

char *ID3_GetString(const ID3_Frame *frame, const ID3_FieldID fldName);
// in 'id3_tag_parse_v1.cpp'
void ID3_RemoveTrailingSpaces(char *buffer, luint length);

#endif

// $Log: misc_support.h,v $
// Revision 1.4  1999/12/05 05:34:41  scott
// Updated the Add functions so that they accept an option boolean
// parameter which indicates if the function should replace the current
// frame, if any.  Made any char * parameters const, if appropriate.
// Added a Remove function for each of the frame types handled in this
// file.  Also generalized AddComment so that it now accepts a Description
// string, which defaults to "".
//
// Revision 1.3  1999/12/02 22:45:28  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places.
//
// Revision 1.2  1999/12/01 22:10:02  scott
// Minor declaraction problem (thanks, elrod).
//
// Revision 1.1  1999/12/01 17:16:08  scott
// moved from src/id3 to include/id3
//
// Revision 1.8  1999/11/30 20:13:09  scott
// Removed include for wchar.h
//
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/29 19:17:33  scott
// Renamed ID3_ASCIItoUnicode to mbstoucs.  Renamed ID3_UnicodeToASCII to
// ucstombs.  Added declarations for ucslen, ucscpy, ucsncpy, ucscmp, and
// ucsncmp.
//
// Revision 1.5  1999/11/19 18:59:09  scott
// * misc_support.h: Added the function declarations for ID3_GetString,
// ID3_GetArtist, ID3_GetAlbum, ID3_GetTitle, ID3_GetYear, ID3_AddYear,
// ID3_GetComment, ID3_AddComment, ID3_GetTrack, ID3_GetTrackNum,
// ID3_AddTrack, ID3_GetGenre, ID3_GetGenreNum, and ID3_GetLyrics.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
