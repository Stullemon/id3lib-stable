// $Id: misc_support.cpp,v 1.6 2000/05/04 20:52:04 eldamitri Exp $

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

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
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
    catch (ID3_Err&)
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

ID3_Frame* ID3_AddArtist(ID3_Tag *tag, const char *text, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
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
      pFrame = new ID3_Frame;
      if (NULL != pFrame)
      {
        pFrame->SetID(ID3FID_LEADARTIST);
        pFrame->Field(ID3FN_TEXT) = text;
        tag->AttachFrame(pFrame);
      }
    }
  }

  return pFrame;
}

size_t ID3_RemoveArtists(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_LEADARTIST)))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }
  while ((pFrame = tag->Find(ID3FID_BAND)))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }
  while ((pFrame = tag->Find(ID3FID_CONDUCTOR)))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }
  while ((pFrame = tag->Find(ID3FID_COMPOSER)))
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

ID3_Frame* ID3_AddAlbum(ID3_Tag *tag, const char *text, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveAlbums(tag);
    }
    if (bReplace || tag->Find(ID3FID_ALBUM) == NULL)
    {
      pFrame = new ID3_Frame;
      if (NULL != pFrame)
      {
        pFrame->SetID(ID3FID_ALBUM);
        pFrame->Field(ID3FN_TEXT) = text;
        tag->AttachFrame(pFrame);
      }
    }
  }
  
  return pFrame;
}

size_t ID3_RemoveAlbums(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_ALBUM)))
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

ID3_Frame* ID3_AddTitle(ID3_Tag *tag, const char *text, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveTitles(tag);
    }
    if (bReplace || tag->Find(ID3FID_TITLE) == NULL)
    {
      pFrame = new ID3_Frame;
      if (NULL == pFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }
      pFrame->SetID(ID3FID_TITLE);
      pFrame->Field(ID3FN_TEXT) = text;
      tag->AttachFrame(pFrame);
    }
  }
  
  return pFrame;
}

size_t ID3_RemoveTitles(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_TITLE)))
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

ID3_Frame* ID3_AddYear(ID3_Tag *tag, const char *text, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveYears(tag);
    }
    if (bReplace || tag->Find(ID3FID_YEAR) == NULL)
    {
      pFrame = new ID3_Frame;
      if (NULL != pFrame)
      {
        pFrame->SetID(ID3FID_YEAR);
        pFrame->Field(ID3FN_TEXT) = text;
        tag->AttachFrame(pFrame);
      }
    }
  }
  
  return pFrame;
}

size_t ID3_RemoveYears(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_YEAR)))
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

ID3_Frame* ID3_AddComment(ID3_Tag *tag, const char *sComment,
                          const char *sDescription, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
  if (NULL != tag          &&
      NULL != sComment     &&
      NULL != sDescription && 
      strlen(sComment) > 0)
  {
    bool bAdd = true;
    if (bReplace)
    {
      ID3_RemoveComments(tag, sDescription);
    }
    else
    {
      // See if there is already a comment with this description
      for (size_t nCount = 0; nCount < tag->NumFrames(); nCount++)
      {
        pFrame = tag->GetFrameNum(nCount);
        if (pFrame->GetID() == ID3FID_COMMENT)
        {
          char *sDesc = ID3_GetString(pFrame, ID3FN_DESCRIPTION);
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
      pFrame = new ID3_Frame;
      if (NULL != pFrame)
      {
        pFrame->SetID(ID3FID_COMMENT);
        pFrame->Field(ID3FN_LANGUAGE) = "eng";
        pFrame->Field(ID3FN_DESCRIPTION) = sDescription;
        pFrame->Field(ID3FN_TEXT) = sComment;
        tag->AttachFrame(pFrame);
      }
    }
  }
  return pFrame;
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

ID3_Frame* ID3_AddTrack(ID3_Tag *tag, uchar ucTrack, uchar ucTotal, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
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
        char *sTrack = NULL;
        if (0 == ucTotal)
        {
          sTrack = new char[4];
          sprintf(sTrack, "%lu", (luint) ucTrack);
        }
        else
        {
          sTrack = new char[8];
          sprintf(sTrack, "%lu/%lu", (luint) ucTrack, (luint) ucTotal);
        }

        trackFrame->SetID(ID3FID_TRACKNUM);
        trackFrame->Field(ID3FN_TEXT) = sTrack;
        tag->AttachFrame(trackFrame);

        delete [] sTrack;
      }
    }
  }
  
  return pFrame;
}

size_t ID3_RemoveTracks(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_TRACKNUM)))
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

ID3_Frame* ID3_AddGenre(ID3_Tag *tag, luint ucGenre, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
  if (NULL != tag && 0xFF != ucGenre)
  {
    if (bReplace)
    {
      ID3_RemoveGenres(tag);
    }
    if (bReplace || NULL == tag->Find(ID3FID_CONTENTTYPE))
    {
      pFrame = new ID3_Frame;
      if (NULL != pFrame)
      {
        char sGenre[6];
        sprintf(sGenre, "(%lu)", (luint) ucGenre);

        pFrame->SetID(ID3FID_CONTENTTYPE);
        pFrame->Field(ID3FN_TEXT) = sGenre;
        tag->AttachFrame(pFrame);
      }
    }
  }
  
  return pFrame;
}

size_t ID3_RemoveGenres(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_CONTENTTYPE)))
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

ID3_Frame* ID3_AddLyrics(ID3_Tag *tag, const char *text, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
  if (NULL != tag && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveLyrics(tag);
    }
    if (bReplace || tag->Find(ID3FID_UNSYNCEDLYRICS) == NULL)
    {
      pFrame = new ID3_Frame;
      if (NULL != pFrame)
      {
        pFrame->SetID(ID3FID_UNSYNCEDLYRICS);
        pFrame->Field(ID3FN_LANGUAGE) = "eng";
        pFrame->Field(ID3FN_TEXT) = text;
        tag->AttachFrame(pFrame);
      }
    }
  }
  
  return pFrame;
}

size_t ID3_RemoveLyrics(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_UNSYNCEDLYRICS)))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}

char *ID3_GetLyricist(ID3_Tag *tag)
{
  char *sLyricist = NULL;
  if (NULL == tag)
  {
    return sLyricist;
  }

  ID3_Frame *pFrame = tag->Find(ID3FID_LYRICIST);
  if (pFrame != NULL)
  {
    sLyricist = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sLyricist;
}

ID3_Frame* ID3_AddLyricist(ID3_Tag *tag, const char *text, bool bReplace)
{
  ID3_Frame* pFrame = NULL;
  if (NULL != tag && NULL != text && strlen(text) > 0)
  {
    if (bReplace)
    {
      ID3_RemoveLyricist(tag);
    }
    if (bReplace || (tag->Find(ID3FID_LYRICIST) == NULL))
    {    
      pFrame = new ID3_Frame;
      if (NULL == pFrame)
      {
        ID3_THROW(ID3E_NoMemory);
      }
      
      pFrame->SetID(ID3FID_LYRICIST);
      pFrame->Field(ID3FN_TEXT) = text;
      tag->AttachFrame(pFrame);
    }
  }

  return pFrame;
}

size_t ID3_RemoveLyricist(ID3_Tag *tag)
{
  size_t nRemoved = 0;
  ID3_Frame *pFrame = NULL;

  if (NULL == tag)
  {
    return nRemoved;
  }

  while ((pFrame = tag->Find(ID3FID_LYRICIST)))
  {
    tag->RemoveFrame(pFrame);
    nRemoved++;
  }

  return nRemoved;
}
