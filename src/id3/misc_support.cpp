// $Id: misc_support.cpp,v 1.6 1999/11/15 20:20:01 scott Exp $

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

// converts an ASCII string into a Unicode one

void ID3_ASCIItoUnicode(wchar_t *unicode, char *ascii, luint len)
{
  if (ascii && unicode)
    for (luint i = 0; i < len; i++)
      unicode[i] = (wchar_t) ascii[i] & 0xFF;
      
  return ;
}

// converts a Unicode string into ASCII

void ID3_UnicodeToASCII(char *ascii, wchar_t *unicode, luint len)
{
  if (unicode && ascii)
    for (luint i = 0; i < len; i++)
      ascii[i] = unicode[i] & 0xFF;
      
  return ;
}

void ID3_AddArtist(ID3_Tag *tag, char *text)
{
  if (tag->Find(ID3FID_LEADARTIST) == NULL &&
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
    tag->AddFrame(artistFrame, true);
  }
  
  return ;
}


void ID3_AddAlbum(ID3_Tag *tag, char *text)
{
  if (NULL == tag->Find(ID3FID_ALBUM) && strlen(text) > 0)
  {
    ID3_Frame *albumFrame;
    
    albumFrame = new ID3_Frame;
    if (NULL == albumFrame)
      ID3_THROW(ID3E_NoMemory);

    albumFrame->SetID(ID3FID_ALBUM);
    albumFrame->Field(ID3FN_TEXT) = text;
    tag->AddFrame(albumFrame, true);
  }
  
  return ;
}


void ID3_AddTitle(ID3_Tag *tag, char *text)
{
  if (NULL == tag->Find(ID3FID_TITLE) && strlen(text) > 0)
  {
    ID3_Frame *titleFrame;
    
    titleFrame = new ID3_Frame;
    if (NULL == titleFrame)
      ID3_THROW(ID3E_NoMemory);

    titleFrame->SetID(ID3FID_TITLE);
    titleFrame->Field(ID3FN_TEXT) = text;
    tag->AddFrame(titleFrame, true);
  }
  
  return ;
}


void ID3_AddLyrics(ID3_Tag *tag, char *text)
{
  if (NULL == tag->Find(ID3FID_UNSYNCEDLYRICS) && strlen(text) > 0)
  {
    ID3_Frame *lyricsFrame;
    
    lyricsFrame = new ID3_Frame;
    if (NULL == lyricsFrame)
      ID3_THROW(ID3E_NoMemory);

    lyricsFrame->SetID(ID3FID_UNSYNCEDLYRICS);
    lyricsFrame->Field(ID3FN_LANGUAGE) = "eng";
    lyricsFrame->Field(ID3FN_TEXT) = text;
    tag->AddFrame(lyricsFrame, true);
  }
  
  return ;
}

// $Log: misc_support.cpp,v $
// Revision 1.6  1999/11/15 20:20:01  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.5  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
