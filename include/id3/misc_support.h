// $Id: misc_support.h,v 1.14 2000/05/06 21:40:25 eldamitri Exp $

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

#ifndef __ID3LIB_MISC_SUPPORT_H__
#define __ID3LIB_MISC_SUPPORT_H__

#include "tag.h"

// defined in 'id3_misc_support.cpp'
// these are 'convenience functions,' to make using the library easier for the 
// most common of operations
char  *ID3_GetArtist(ID3_Tag *tag);
ID3_Frame* ID3_AddArtist(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveArtists(ID3_Tag *tag);

char  *ID3_GetAlbum(ID3_Tag *tag);
ID3_Frame* ID3_AddAlbum(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveAlbums(ID3_Tag *tag);

char  *ID3_GetTitle(ID3_Tag *tag);
ID3_Frame* ID3_AddTitle(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveTitles(ID3_Tag *tag);

char  *ID3_GetYear(ID3_Tag *tag);
ID3_Frame* ID3_AddYear(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveYears(ID3_Tag *tag);

char  *ID3_GetComment(ID3_Tag *tag);
ID3_Frame* ID3_AddComment(ID3_Tag *tag, const char *text,
                          const char *sDescription = "", bool bReplace = false);
size_t ID3_RemoveComments(ID3_Tag *tag, const char *sDescription = NULL);

char  *ID3_GetTrack(ID3_Tag *tag);
luint  ID3_GetTrackNum(ID3_Tag *tag);
ID3_Frame* ID3_AddTrack(ID3_Tag *tag, uchar ucTrack, uchar ucTotal = 0,
                        bool bReplace = false);
ID3_Frame* ID3_AddTrack(ID3_Tag *tag, uchar ucTrack, bool bReplace);
size_t ID3_RemoveTracks(ID3_Tag *tag);

char  *ID3_GetGenre(ID3_Tag *tag);
luint  ID3_GetGenreNum(ID3_Tag *tag);
ID3_Frame* ID3_AddGenre(ID3_Tag *tag, luint ucGenre, bool bReplace = false);
size_t ID3_RemoveGenres(ID3_Tag *tag);

char  *ID3_GetLyrics(ID3_Tag *tag);
ID3_Frame* ID3_AddLyrics(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveLyrics(ID3_Tag *tag);

char  *ID3_GetLyricist(ID3_Tag *tag);
ID3_Frame* ID3_AddLyricist(ID3_Tag *tag, const char *text, bool bReplace = false);
size_t ID3_RemoveLyricist(ID3_Tag *tag);

char *ID3_GetString(const ID3_Frame *frame, const ID3_FieldID fldName,
                    const size_t nItems = 1);

#endif /* __ID3LIB_MISC_SUPPORT_H__ */
