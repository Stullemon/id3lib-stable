// $Id: misc_support.cpp,v 1.19 2000/01/04 15:42:49 eldamitri Exp $
// 
// This program is free software; you can distribute it and/or modify it under
// the terms discussed in the COPYING file, which should have been included
// with this distribution.
//  
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the COPYING file for more details.
//  
// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where
// to send such submissions.

#include <cstdlib>
#include <cctype>
#include <cstdio>
#include "misc_support.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

// Extract a 32-bit number from a 4-byte character array
size_t ParseNumber(const uchar *buffer, size_t size)
{
  size_t nSize = 0;
  for (size_t nIndex = 0; nIndex < size; nIndex++)
  {
    nSize |= buffer[nIndex] << ((size - nIndex - 1) * 8);
  }
  return nSize;
}

void RenderNumber(uchar *buffer, size_t val, size_t size)
{
  for (size_t nIndex = 0; nIndex < size; nIndex++)
  {
    buffer[nIndex] = (uchar)((val >> ((size - nIndex - 1) * 8)) & MASK8);
  }
}

// converts an ASCII string into a Unicode one

void mbstoucs(unicode_t *unicode, const char *ascii, const luint len)
{
  if (NULL != ascii && NULL != unicode)
  {
    for (luint i = 0; i < len; i++)
    {
      unicode[i] = ascii[i] & 0xFF;
    }
  }
}

// converts a Unicode string into ASCII

void ucstombs(char *ascii, const unicode_t *unicode, const luint len)
{
  if (NULL != unicode && NULL != ascii)
  {
    for (luint i = 0; i < len; i++)
    {
      ascii[i] = unicode[i] & 0x00FF;
    }
  }
}

size_t ucslen(const unicode_t *unicode)
{
  if (NULL != unicode)
  {
    for (size_t size = 0; true; size++)
    {
      if (NULL_UNICODE == unicode[size])
      {
        return size;
      }
    }
  }
  return 0;
}

void ucscpy(unicode_t *dest, const unicode_t *src)
{
  if (NULL != dest && NULL != src)
  {
    size_t nIndex;
    for (nIndex = 0; NULL_UNICODE != src[nIndex]; nIndex++)
    {
      dest[nIndex] = src[nIndex];
    }
    dest[nIndex] = NULL_UNICODE;
  }
}

void ucsncpy(unicode_t *dest, const unicode_t *src, size_t len)
{
  if (NULL != dest && NULL != src)
  {
    size_t nIndex;
    for (nIndex = 0; nIndex < len && NULL_UNICODE != src[nIndex]; nIndex++)
    {
      dest[nIndex] = src[nIndex];
    }
    for (; nIndex < len; nIndex++)
    {
      dest[nIndex] = NULL_UNICODE;
    }
  }
}

int ucscmp(const unicode_t *s1, const unicode_t *s2)
{
  return ucsncmp(s1, s2, (size_t) -1);
}

int ucsncmp(const unicode_t *s1, const unicode_t *s2, size_t len)
{
  if (NULL == s1 && NULL == s2)
  {
    return  0;
  }
  if (NULL == s1)
  {
    return  1;
  }
  if (NULL == s2)
  {
    return -1;
  }
  for (size_t nIndex = 0; true; nIndex++)
  {
    if ((NULL_UNICODE == s1[nIndex]) ||
        (NULL_UNICODE == s2[nIndex]) ||
        (s1[nIndex]   != s2[nIndex]) ||
        (nIndex + 1   == len))
    {
      return s2[nIndex] - s1[nIndex];
    }
  }
}

char *ID3_GetString(const ID3_Frame *frame, const ID3_FieldID fldName,
                    const size_t nIndex)
{
  char *sText = NULL;
  if (NULL != frame)
  {
    size_t nText = frame->Field(fldName).Size();
    sText = new char[nText + 1];
    try 
    {
      frame->Field(fldName).Get(sText, nText, nIndex);
    }
    catch (ID3_Err err)
    {
      delete [] sText;
      return NULL;
    }
    sText[nText] = '\0';
  }
  return sText;
}

char *ID3_GetArtist(ID3_Tag *tag)
{
  char *sArtist = NULL;
  if (NULL == tag)
  {
    return sArtist;
  }

  ID3_Frame *pFrame = NULL;
  if ((pFrame = tag->Find(ID3FID_LEADARTIST)) ||
      (pFrame = tag->Find(ID3FID_BAND))       ||
      (pFrame = tag->Find(ID3FID_CONDUCTOR))  ||
      (pFrame = tag->Find(ID3FID_COMPOSER)))
  {
    sArtist = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sArtist;
}

bool ID3_AddArtist(ID3_Tag *tag, const char *text, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveArtists(tag);
    }
    if (bReplace ||
        (tag->Find(ID3FID_LEADARTIST) == NULL &&
         tag->Find(ID3FID_BAND)       == NULL &&
         tag->Find(ID3FID_CONDUCTOR)  == NULL &&
         tag->Find(ID3FID_COMPOSER)   == NULL))
    {
      ID3_Frame *artistFrame;
      
      artistFrame = new ID3_Frame;
      if (NULL == artistFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }
      
      artistFrame->SetID(ID3FID_LEADARTIST);
      artistFrame->Field(ID3FN_TEXT) = text;
      tag->AddNewFrame(artistFrame);
    }
  }

  return bSuccess;
}

size_t ID3_RemoveArtists(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while (pFrame = tag->Find(ID3FID_LEADARTIST))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }
  while (pFrame = tag->Find(ID3FID_BAND))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }
  while (pFrame = tag->Find(ID3FID_CONDUCTOR))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }
  while (pFrame = tag->Find(ID3FID_COMPOSER))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

char *ID3_GetAlbum(ID3_Tag *tag)
{
  char *sAlbum = NULL;
  if (NULL == tag)
  {
    return sAlbum;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_ALBUM);
  if (pFrame != NULL)
  {
    sAlbum = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sAlbum;
}

bool ID3_AddAlbum(ID3_Tag *tag, const char *text, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveAlbums(tag);
    }
    if (bReplace || tag->Find(ID3FID_ALBUM) == NULL)
    {
      ID3_Frame *albumFrame;
      
      albumFrame = new ID3_Frame;
      if (NULL == albumFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }
      
      albumFrame->SetID(ID3FID_ALBUM);
      albumFrame->Field(ID3FN_TEXT) = text;
      tag->AddNewFrame(albumFrame);
      
      bSuccess = true;
    }
  }
  
  return bSuccess;
}

size_t ID3_RemoveAlbums(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while (pFrame = tag->Find(ID3FID_ALBUM))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

char *ID3_GetTitle(ID3_Tag *tag)
{
  char *sTitle = NULL;
  if (NULL == tag)
  {
    return sTitle;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_TITLE);
  if (pFrame != NULL)
  {
    sTitle = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sTitle;
}

bool ID3_AddTitle(ID3_Tag *tag, const char *text, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveTitles(tag);
    }
    if (bReplace || tag->Find(ID3FID_TITLE) == NULL)
    {
      ID3_Frame *titleFrame;
      
      titleFrame = new ID3_Frame;
      if (NULL == titleFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }
      
      titleFrame->SetID(ID3FID_TITLE);
      titleFrame->Field(ID3FN_TEXT) = text;
      tag->AddNewFrame(titleFrame);
      
      bSuccess = true;
    }
  }
  
  return bSuccess;
}

size_t ID3_RemoveTitles(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while (pFrame = tag->Find(ID3FID_TITLE))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

char *ID3_GetYear(ID3_Tag *tag)
{
  char *sYear = NULL;
  if (NULL == tag)
  {
    return sYear;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_YEAR);
  if (pFrame != NULL)
  {
    sYear = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sYear;
}

bool ID3_AddYear(ID3_Tag *tag, const char *text, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveYears(tag);
    }
    if (bReplace || tag->Find(ID3FID_YEAR) == NULL)
    {
      ID3_Frame *yearFrame;
    
      yearFrame = new ID3_Frame;
      if (NULL == yearFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }

      yearFrame->SetID(ID3FID_YEAR);
      yearFrame->Field(ID3FN_TEXT) = text;
      tag->AddNewFrame(yearFrame);

      bSuccess = true;
    }
  }
  
  return bSuccess;
}

size_t ID3_RemoveYears(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while (pFrame = tag->Find(ID3FID_YEAR))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

char *ID3_GetComment(ID3_Tag *tag)
{
  char *sComment = NULL;
  if (NULL == tag)
  {
    return sComment;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_COMMENT);
  if (pFrame != NULL)
  {
    sComment = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sComment;
}

bool ID3_AddComment(ID3_Tag *tag, const char *sComment,
                    const char *sDescription, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag          &&
      NULL != sComment     &&
      NULL != sDescription && 
      strlen(sComment) > 0)
  {
    bool bAdd = true;
    ID3_Frame *frame;
    if (bReplace)
    {
      ID3_RemoveComments(tag, sDescription);
    }
    else
    {
      // See if there is already a comment with this description
      for (size_t nCount = 0; nCount < tag->NumFrames(); nCount++)
      {
        frame = tag->GetFrameNum(nCount);
        if (frame->GetID() == ID3FID_COMMENT)
        {
          char *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION);
          if (strcmp(sDesc, sDescription) == 0)
          {
            bAdd = false;
          }
          delete [] sDesc;
          if (!bAdd)
          {
            break;
          }
        }
      }
    }
    if (bAdd)
    {
      frame = new ID3_Frame;
      if (NULL == frame)
        ID3_THROW(ID3E_NoMemory);

      frame->SetID(ID3FID_COMMENT);
      frame->Field(ID3FN_LANGUAGE) = "eng";
      frame->Field(ID3FN_DESCRIPTION) = sDescription;
      frame->Field(ID3FN_TEXT) = sComment;
      tag->AddNewFrame(frame);
    }
  }
  return bSuccess;
}

// Remove all comments with the given description (remove all comments if
// sDescription is NULL)
size_t ID3_RemoveComments(ID3_Tag *tag, const char *sDescription)
{
  size_t nRemoved = 0;

  if (NULL == tag)
  {
    return nRemoved;
  }

  for (size_t nCount = 0; nCount < tag->NumFrames(); nCount++)
  {
    ID3_Frame *pFrame = tag->GetFrameNum(nCount);
    if (pFrame->GetID() == ID3FID_COMMENT)
    {
      bool bRemove = false;
      // A null description means remove all comments
      if (NULL == sDescription)
      {
        bRemove = true;
      }
      else
      {
        // See if the description we have matches the description of the 
        // current comment.  If so, set the "remove the comment" flag to true.
        char *sDesc = ID3_GetString(pFrame, ID3FN_DESCRIPTION);
        if (strcmp(sDesc, sDescription) == 0)
        {
          bRemove = true;
        }
        delete [] sDesc;
      }
      if (bRemove)
      {
        tag->RemoveFrame(pFrame);
        nRemoved++;
      }
    }
  }

  return nRemoved;
}

char *ID3_GetTrack(ID3_Tag *tag)
{
  char *sTrack = NULL;
  if (NULL == tag)
  {
    return sTrack;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_TRACKNUM);
  if (pFrame != NULL)
  {
    sTrack = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sTrack;
}

luint ID3_GetTrackNum(ID3_Tag *tag)
{
  char *sTrack = ID3_GetTrack(tag);
  luint nTrack = 0;
  if (NULL != sTrack)
  {
    nTrack = atoi(sTrack);
    delete [] sTrack;
  }
  return nTrack;
}

bool ID3_AddTrack(ID3_Tag *tag, uchar ucTrack, uchar ucTotal, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag && ucTrack > 0)
  {
    if (bReplace)
    {
      ID3_RemoveTracks(tag);
    }
    if (bReplace || NULL == tag->Find(ID3FID_TRACKNUM))
    {
      ID3_Frame *trackFrame;
    
      trackFrame = new ID3_Frame;
      if (NULL == trackFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }

      char *sTrack = NULL;
      if (0 == ucTotal)
      {
        sTrack = new char[4];
        sprintf(sTrack, "%lu", ucTrack);
      }
      else
      {
        sTrack = new char[8];
        sprintf(sTrack, "%lu/%lu", ucTrack, ucTotal);
      }

      trackFrame->SetID(ID3FID_TRACKNUM);
      trackFrame->Field(ID3FN_TEXT) = sTrack;
      tag->AddNewFrame(trackFrame);

      delete [] sTrack;
    
      bSuccess = true;
    }
  }
  
  return bSuccess;
}

size_t ID3_RemoveTracks(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while (pFrame = tag->Find(ID3FID_TRACKNUM))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

char *ID3_GetGenre(ID3_Tag *tag)
{
  char *sGenre = NULL;
  if (NULL == tag)
  {
    return sGenre;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_CONTENTTYPE);
  if (pFrame != NULL)
  {
    sGenre = ID3_GetString(pFrame, ID3FN_TEXT);
  }

  return sGenre;
}

luint ID3_GetGenreNum(ID3_Tag *tag)
{
  char *sGenre = ID3_GetGenre(tag);
  luint ulGenre = 0xFF;
  if (NULL == sGenre)
  {
    return ulGenre;
  }

  // If the genre string begins with "(ddd)", where "ddd" is a number, then 
  // "ddd" is the genre number---get it
  if (sGenre[0] == '(')
  {
    char *pCur = &sGenre[1];
    while (isdigit(*pCur))
    {
      pCur++;
    }
    if (*pCur == ')')
    {
      // if the genre number is greater than 255, its invalid.
      ulGenre = MIN(0xFF, atoi(&sGenre[1]));
    }
  }

  delete [] sGenre;
  return ulGenre;
}

bool ID3_AddGenre(ID3_Tag *tag, luint ucGenre, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag && 0xFF != ucGenre)
  {
    if (bReplace)
    {
      ID3_RemoveGenres(tag);
    }
    if (bReplace || NULL == tag->Find(ID3FID_CONTENTTYPE))
    {
      ID3_Frame *pFrame;
    
      pFrame = new ID3_Frame;
      if (NULL == pFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }

      char sGenre[6];
      sprintf(sGenre, "(%lu)", (luint) ucGenre);

      pFrame->SetID(ID3FID_CONTENTTYPE);
      pFrame->Field(ID3FN_TEXT) = sGenre;
      tag->AddNewFrame(pFrame);

      bSuccess = true;
    }
  }
  
  return bSuccess;
}

size_t ID3_RemoveGenres(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while (pFrame = tag->Find(ID3FID_CONTENTTYPE))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

char *ID3_GetLyrics(ID3_Tag *tag)
{
  char *sLyrics = NULL;
  if (NULL == tag)
  {
    return sLyrics;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_UNSYNCEDLYRICS);
  if (pFrame != NULL)
  {
    sLyrics = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sLyrics;
}

bool ID3_AddLyrics(ID3_Tag *tag, const char *text, bool bReplace)
{
  bool bSuccess = false;
  if (NULL != tag && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveLyrics(tag);
    }
    if (bReplace || tag->Find(ID3FID_UNSYNCEDLYRICS) == NULL)
    {
      ID3_Frame *lyricsFrame;
    
      lyricsFrame = new ID3_Frame;
      if (NULL == lyricsFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }

      lyricsFrame->SetID(ID3FID_UNSYNCEDLYRICS);
      lyricsFrame->Field(ID3FN_LANGUAGE) = "eng";
      lyricsFrame->Field(ID3FN_TEXT) = text;
      tag->AddNewFrame(lyricsFrame);
    
      bSuccess = true;
    }
  }
  
  return bSuccess;
}

size_t ID3_RemoveLyrics(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while (pFrame = tag->Find(ID3FID_UNSYNCEDLYRICS))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

// $Log: misc_support.cpp,v $
// Revision 1.19  2000/01/04 15:42:49  eldamitri
// * include/id3/field.h:
// * include/id3/int28.h:
// * include/id3/misc_support.h:
// * include/id3/tag.h:
// * include/id3/types.h:
// * src/id3/dll_wrapper.cpp
// * src/id3/error.cpp
// * src/id3/field.cpp
// * src/id3/field_binary.cpp
// * src/id3/field_integer.cpp
// * src/id3/field_string_ascii.cpp
// * src/id3/field_string_unicode.cpp
// * src/id3/frame.cpp
// * src/id3/frame_parse.cpp
// * src/id3/frame_render.cpp
// * src/id3/header.cpp
// * src/id3/header_frame.cpp
// * src/id3/header_tag.cpp
// * src/id3/int28.cpp
// * src/id3/misc_support.cpp
// * src/id3/tag.cpp
// * src/id3/tag_file.cpp:
// * src/id3/tag_find.cpp:
// * src/id3/tag_parse.cpp:
// * src/id3/tag_parse_lyrics3.cpp:
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.18  1999/12/26 15:11:51  scott
// (ParseNumber): Defined.  Converts a character buffer up to 4 bytes in
// size into its equivalent big-endian integer equivalent.
// (RenderNumber): Defined.  Converts an integer into an equivalent
// big-endian binary character array, up to 4 bytes in length.
//
// Revision 1.17  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.16  1999/12/13 04:44:12  scott
// (ID3_AddComment): Fixed bug that wasn't adding specified comment
// description to comment.
//
// Revision 1.15  1999/12/09 03:32:22  scott
// (ID3_GetString): Added implementation for new nIndex parameter, which
// will pull out the nIndex'th string from the field, such as for the
// involved people list frame.
// (ID3_GetGenre): Fixed bug that wasn't finding the track num frame
// rather than the genre frame (thanks Alexander Voronin).
//
// Revision 1.14  1999/12/05 05:34:34  scott
// (ID3_AddArtist): Added extra boolean parameter (see changes for
// misc_support.h above) as well as the functionality that implements the
// feature.  If bReplace is true, than the new information replaces the
// information in the previous Artist frame, if any.  If bReplace is
// false, the information is added only if no previous Artist frame
// existed.
// (ID3_AddAlbum): See changes for ID3_AddArtist.
// (ID3_AddTitle): See changes for ID3_AddArtist.
// (ID3_AddYear): See changes for ID3_AddArtist.
// (ID3_AddComment): See changes for ID3_AddArtist.  Also generalized
// implementaiton using the new sDescription parameter.
// (ID3_AddTrack): See changes for ID3_AddArtist.
// (ID3_AddGenre): See changes for ID3_AddArtist.
// (ID3_AddLyrics): See changes for ID3_AddArtist.
// (ID3_RemoveArtists): Added.  Removes any and all frames that are
// associated with artist information (namely, ID3FID_LEADARTIST,
// ID3FID_BAND, ID3FID_CONDUCTOR, and ID3FID_COMPOSER) from the tag.
// (ID3_RemoveAlbums): Added.  Removes any and all album name frames
// (ID3_RemoveTitles): Added.  Removes any and all song title frames
// (ID3_RemoveYears): Added.  Removes any and all year frames
// (ID3_RemoveComments): Added.  Removes any and all comment frames that
// match the sDescription parameter.  If sDescription is NULL, all comment
// frames are removed.
// (ID3_RemoveTracks): Added.  Removes any and all track number frames
// (ID3_RemoveGenres): Added.  Removes any and all content type frames
// (ID3_RemoveLyrics): Added.  Removes any and all unsynced lyrics frames
//
// Revision 1.13  1999/12/01 22:11:50  scott
// Now returns 0 when input is NULL.
//
// Revision 1.12  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.11  1999/11/30 20:12:35  scott
// No more SIZE_SPECIFIC_TYPES code.  No more code for wchar's.  Typedefs
// in sized_types.h guarantee that unicode_t will be two bytes.
//
// Revision 1.10  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.9  1999/11/29 18:56:37  scott
// (): Made includsion of wchar.h dependant on SIZE_SPECIFIC_TYPES.
// Put in compile-time checks to compile differently based on the
// definition of unicode_t.
// (mbstoucs): Renamed from ID3_ASCIItoUnicode.
// (ucstombs): Renamed from ID3_UnicodeToASCII.
// (ucslen): Added.  Returns the length of a unicode character string.
// (ucscpy): Added.  Copies one unicode string to another.
// (ucsncpy): Added.  Copies n chars from one unicode string to another.
// (ucscmp): Added.  Compares two unicode strings.
// (ucsncmp): Added.  Compares the first n chars of two unicode strings.
//
// Revision 1.8  1999/11/25 18:52:40  scott
// * misc_support.cpp: Replaced every call to AddFrame with AddNewFrame.
//
// Revision 1.7  1999/11/19 18:53:16  scott
// (ID3_ASCIItoUnicode): Updated interface to make parameters const.
// Replaced content of the code with call to mbstowcs, a function defined
// in wchar.h for converting from a regular string to a wchar string.  The
// original code might be reinstated if another type is used to store
// unicode characters.
// (ID3_UnicodeToASCII): Updated interface to make parameters const.
// Replaced content of the code with call to wcstombs, a function defined
// in wchar.h for converting from a wchar string to a regular string.  The
// original code might be reinstated if another type is used to store
// unicode characters.
// (ID3_GetString): Added.  Returns a dynamically-allocated copy of the
// string contained in the frame/field parameters.
// (ID3_GetArtist): Added.  Returns the artist as a string.  Will return
// the LEADARTIST, the BAND, the CONDUCTOR, or the COMPOSER, whichever is
// found first in that order.
// (ID3_GetAlbum): Added.  Returns the album as a string.
// (ID3_GetTitle): Added.  Returns the song title as a string.
// (ID3_GetYear): Added.  Returns the year as a string.
// (ID3_AddYear): Added.  Adds the year as a string to the frame.
// (ID3_GetComment): Added.  Returns the first comment found as a string.
// (ID3_AddComment): Added. Adds the comment as a string with the
// description stored in STR_V1_COMMENT_DESC, currently "ID3v1_Comment".
// Won't add if a tag with that or no description already exists.
// (ID3_GetTrack): Added.  Returns the track number as a string.
// (ID3_GetTrackNum): Added.  Returns the track number as an unsigned
// integer.
// (ID3_AddTrack): Added.  Adds the track number and track total as a
// string in the form "N/T", where N is the track number and T is the
// total.  If no total is indicated, then only the track number is added.
// (ID3_GetGenre): Added.  Returns the genre as a string.
// (ID3_GetGenreNum): Added. Returns the genre as a number.
// (ID3_AddGenre): Added.  Adds the genre number as a string in the form
// "(G)", where G is the genre number.
// (ID3_GetLyrics): Added.  Gets the unsynced lyrics as a string.
//
// Revision 1.6  1999/11/15 20:20:01  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.5  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
