// $Id: misc_support.cpp,v 1.8 1999/11/25 18:52:40 scott Exp $

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

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <id3/misc_support.h>
#include <stdlib.h>
#include <ctype.h>

// converts an ASCII string into a Unicode one

void ID3_ASCIItoUnicode(wchar_t *unicode, const char *ascii, const luint len)
{
  mbstowcs(unicode, ascii, len);
  //if (NULL != ascii && NULL != unicode)
  //for (luint i = 0; i < len; i++)
  //unicode[i] = (wchar_t) ascii[i] & 0xFF;
      
  return ;
}

// converts a Unicode string into ASCII

void ID3_UnicodeToASCII(char *ascii, const wchar_t *unicode, const luint len)
{
  wcstombs(ascii, unicode, len);
  //if (NULL != unicode && NULL != ascii)
  //for (luint i = 0; i < len; i++)
  //ascii[i] = unicode[i] & 0xFF;
      
  return ;
}

char *ID3_GetString(const ID3_Frame *frame, const ID3_FieldID fldName)
{
  char *sText = NULL;
  if (NULL != frame)
  {
    size_t nText = frame->Field(fldName).Size();
    sText = new char[nText + 1];
    try 
    {
      frame->Field(fldName).Get(sText, nText);
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
    return sArtist;

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

bool ID3_AddArtist(ID3_Tag *tag, char *text)
{
  bool bReturn = false;
  if (tag                          != NULL &&
      tag->Find(ID3FID_LEADARTIST) == NULL &&
      tag->Find(ID3FID_BAND)       == NULL &&
      tag->Find(ID3FID_CONDUCTOR)  == NULL &&
      tag->Find(ID3FID_COMPOSER)   == NULL &&
      strlen(text) > 0)
  {
    ID3_Frame *artistFrame;

    artistFrame = new ID3_Frame;
    if (NULL == artistFrame)
      ID3_THROW(ID3E_NoMemory);

    artistFrame->SetID(ID3FID_LEADARTIST);
    artistFrame->Field(ID3FN_TEXT) = text;
    tag->AddNewFrame(artistFrame);
  }
  
  return bReturn;
}

char *ID3_GetAlbum(ID3_Tag *tag)
{
  char *sAlbum = NULL;
  if (NULL == tag)
    return sAlbum;

  ID3_Frame *pFrame = tag->Find(ID3FID_ALBUM);
  if (pFrame != NULL)
  {
    sAlbum = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sAlbum;
}

bool ID3_AddAlbum(ID3_Tag *tag, char *text)
{
  bool bReturn = false;
  if (NULL != tag && NULL == tag->Find(ID3FID_ALBUM) && strlen(text) > 0)
  {
    ID3_Frame *albumFrame;
    
    albumFrame = new ID3_Frame;
    if (NULL == albumFrame)
      ID3_THROW(ID3E_NoMemory);

    albumFrame->SetID(ID3FID_ALBUM);
    albumFrame->Field(ID3FN_TEXT) = text;
    tag->AddNewFrame(albumFrame);

    bReturn = true;
  }
  
  return bReturn;
}

char *ID3_GetTitle(ID3_Tag *tag)
{
  char *sTitle = NULL;
  if (NULL == tag)
    return sTitle;

  ID3_Frame *pFrame = tag->Find(ID3FID_TITLE);
  if (pFrame != NULL)
  {
    sTitle = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sTitle;
}

bool ID3_AddTitle(ID3_Tag *tag, char *text)
{
  bool bReturn = false;
  if (NULL != tag && NULL == tag->Find(ID3FID_TITLE) && strlen(text) > 0)
  {
    ID3_Frame *titleFrame;
    
    titleFrame = new ID3_Frame;
    if (NULL == titleFrame)
      ID3_THROW(ID3E_NoMemory);

    titleFrame->SetID(ID3FID_TITLE);
    titleFrame->Field(ID3FN_TEXT) = text;
    tag->AddNewFrame(titleFrame);

    bReturn = true;
  }
  
  return bReturn;
}

char *ID3_GetYear(ID3_Tag *tag)
{
  char *sYear = NULL;
  if (NULL == tag)
    return sYear;

  ID3_Frame *pFrame = tag->Find(ID3FID_YEAR);
  if (pFrame != NULL)
  {
    sYear = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sYear;
}

bool ID3_AddYear(ID3_Tag *tag, char *text)
{
  bool bReturn = false;
  if (NULL != tag && NULL == tag->Find(ID3FID_YEAR) && strlen(text) > 0)
  {
    ID3_Frame *yearFrame;
    
    yearFrame = new ID3_Frame;
    if (NULL == yearFrame)
      ID3_THROW(ID3E_NoMemory);

    yearFrame->SetID(ID3FID_YEAR);
    yearFrame->Field(ID3FN_TEXT) = text;
    tag->AddNewFrame(yearFrame);

    bReturn = true;
  }
  
  return bReturn;
}

char *ID3_GetComment(ID3_Tag *tag)
{
  char *sComment = NULL;
  if (NULL == tag)
    return sComment;

  ID3_Frame *pFrame = tag->Find(ID3FID_COMMENT);
  if (pFrame != NULL)
  {
    sComment = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sComment;
}

bool ID3_AddComment(ID3_Tag *tag, char *sComment)
{
  bool bReturn = false;
  if (NULL != tag && strlen(sComment) > 0)
  {
    bool bFound = false;
    ID3_Frame *frame;
    // See if there is already an id3v1 comment
    for (size_t nCount = 0; nCount < tag->NumFrames(); nCount++)
    {
      frame = tag->GetFrameNum(nCount);
      if (frame->GetID() == ID3FID_COMMENT)
      {
        char *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION);
        if (strcmp(sDesc, STR_V1_COMMENT_DESC) == 0 ||
            strcmp(sDesc, "") == 0)
        {
          bFound = true;
          delete [] sDesc;
          break;
        }
        delete [] sDesc;
      }
    }
    if (!bFound)
    {
      frame = new ID3_Frame;
      if (NULL == frame)
        ID3_THROW(ID3E_NoMemory);

      frame->SetID(ID3FID_COMMENT);
      frame->Field(ID3FN_LANGUAGE) = "eng";
      frame->Field(ID3FN_DESCRIPTION) = STR_V1_COMMENT_DESC;
      frame->Field(ID3FN_TEXT) = sComment;
      tag->AddNewFrame(frame);
    }
  }
  return bReturn;
}

char *ID3_GetTrack(ID3_Tag *tag)
{
  char *sTrack = NULL;
  if (NULL == tag)
    return sTrack;

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

bool ID3_AddTrack(ID3_Tag *tag, uchar ucTrack, uchar ucTotal)
{
  bool bReturn = false;
  if (NULL == tag->Find(ID3FID_TRACKNUM) && ucTrack > 0)
  {
    ID3_Frame *trackFrame;
    
    trackFrame = new ID3_Frame;
    if (NULL == trackFrame)
      ID3_THROW(ID3E_NoMemory);

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
    
    bReturn = true;
  }
  
  return bReturn;
}

char *ID3_GetGenre(ID3_Tag *tag)
{
  char *sGenre = NULL;
  if (NULL == tag)
    return sGenre;

  ID3_Frame *pFrame = tag->Find(ID3FID_TRACKNUM);
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
    return ulGenre;

  // If the genre string begins with "(ddd)", where "ddd" is a number, then 
  // "ddd" is the genre number---get it
  if (sGenre[0] == '(')
  {
    char *pCur = &sGenre[1];
    while (isdigit(*pCur))
      pCur++;
    if (*pCur == ')')
      // if the genre number is greater than 255, its invalid.
      ulGenre = MIN(0xFF, atoi(&sGenre[1]));
  }

  delete [] sGenre;
  return ulGenre;
}

bool ID3_AddGenre(ID3_Tag *tag, luint ucGenre)
{
  bool bReturn = false;
  if (ucGenre != 0xFF && NULL == tag->Find(ID3FID_CONTENTTYPE))
  {
    ID3_Frame *pFrame;
    
    pFrame = new ID3_Frame;
    if (NULL == pFrame)
      ID3_THROW(ID3E_NoMemory);

    char sGenre[6];
    sprintf(sGenre, "(%lu)", (luint) ucGenre);

    pFrame->SetID(ID3FID_CONTENTTYPE);
    pFrame->Field(ID3FN_TEXT) = sGenre;
    tag->AddNewFrame(pFrame);

    bReturn = true;
  }
  
  return bReturn;
}

char *ID3_GetLyrics(ID3_Tag *tag)
{
  char *sLyrics = NULL;
  if (NULL == tag)
    return sLyrics;

  ID3_Frame *pFrame = tag->Find(ID3FID_UNSYNCEDLYRICS);
  if (pFrame != NULL)
  {
    sLyrics = ID3_GetString(pFrame, ID3FN_TEXT);
  }
  return sLyrics;
}

bool ID3_AddLyrics(ID3_Tag *tag, char *text)
{
  bool bReturn = false;
  if (NULL == tag->Find(ID3FID_UNSYNCEDLYRICS) && strlen(text) > 0)
  {
    ID3_Frame *lyricsFrame;
    
    lyricsFrame = new ID3_Frame;
    if (NULL == lyricsFrame)
      ID3_THROW(ID3E_NoMemory);

    lyricsFrame->SetID(ID3FID_UNSYNCEDLYRICS);
    lyricsFrame->Field(ID3FN_LANGUAGE) = "eng";
    lyricsFrame->Field(ID3FN_TEXT) = text;
    tag->AddNewFrame(lyricsFrame);
    
    bReturn = true;
  }
  
  return bReturn;
}

// $Log: misc_support.cpp,v $
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
