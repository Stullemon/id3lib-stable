// $Id: tag_parse_v1.cpp,v 1.5 1999/11/15 20:21:15 scott Exp $

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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <id3/tag.h>
#include <id3/misc_support.h>

void ID3_RemoveTrailingSpaces(char *buffer, luint length)
{
  for (lsint i = length - 1; i > -1 && 0x20 == buffer[i]; i--)
    buffer[i] = 0;
      
  return ;
}


void ID3_Tag::ParseID3v1(void)
{
  if (NULL == __fFileHandle)
    ID3_THROW(ID3E_NoData);

  ID3V1_Tag temp;
    
  // posn ourselves at 128 bytes from the end of the file
  fseek(__fFileHandle, -128, SEEK_END);
    
  // read the next 128 bytes in;
  fread(&temp, 1, 128, __fFileHandle);
    
  ID3_RemoveTrailingSpaces(temp.sTitle,  sizeof(temp.sTitle));
  ID3_RemoveTrailingSpaces(temp.sArtist, sizeof(temp.sArtist));
  ID3_RemoveTrailingSpaces(temp.sAlbum,  sizeof(temp.sAlbum));
    
  if (0 == temp.sComment[28] && 0 != temp.sComment[29])
    ID3_RemoveTrailingSpaces(temp.sComment, sizeof(temp.sComment) - 1);
  else
    ID3_RemoveTrailingSpaces(temp.sComment, sizeof(temp.sComment));
      
  // check to see if it was a tag
  if (memcmp(temp.sID, "TAG", 3) == 0)
  {
    // guess so, let's start checking the v2 tag for frames which are the
    // equivalent of the v1 fields.  When we come across a v1 field that has
    // no current equivalent v2 frame, we create the frame, copy the data
    // from the v1 frame and attach it to the tag
      
    __bHasV1Tag = true;
    __ulExtraBytes += sizeof(ID3V1_Tag);
      
    ID3_AddTitle(this, temp.sTitle);
    ID3_AddArtist(this, temp.sArtist);
    ID3_AddAlbum(this, temp.sAlbum);
      
    // the YEAR field/frame
    if (NULL == Find(ID3FID_YEAR) && 0 != temp.sYear[0])
    {
      ID3_Frame *yearFrame;
      
      yearFrame = new ID3_Frame;
      if (NULL == yearFrame)
        ID3_THROW(ID3E_NoMemory);

      char buff[5];
      
      strncpy(buff, temp.sYear, 4);
      buff[4] = 0;
      
      yearFrame->SetID(ID3FID_YEAR);
      yearFrame->Field(ID3FN_TEXT) = buff;
      AddFrame(yearFrame, true);
    }
      
    // the COMMENT field/frame
    if (NULL == Find(ID3FID_COMMENT) && strlen(temp.sComment) > 0)
    {
      ID3_Frame *commentFrame;
      
      commentFrame = new ID3_Frame;
      if (NULL == commentFrame)
        ID3_THROW(ID3E_NoMemory);

      commentFrame->SetID(ID3FID_COMMENT);
      commentFrame->Field(ID3FN_LANGUAGE) = "eng";
      //commentFrame->Field(ID3FN_DESCRIPTION) = "";
      commentFrame->Field(ID3FN_TEXT) = temp.sComment;
      AddFrame(commentFrame, true);
    }
      
    // the TRACK field/frame
    if (0 == temp.sComment[28] && temp.sComment[29] != 0)
    {
      if (NULL == Find(ID3FID_TRACKNUM))
      {
        ID3_Frame *trackFrame;
        
        trackFrame = new ID3_Frame;
        if (NULL == trackFrame)
          ID3_THROW(ID3E_NoMemory);

        char buff[4];
        
        sprintf(buff, "%lud", (luint) temp.sComment[29]);
        
        trackFrame->SetID(ID3FID_TRACKNUM);
        trackFrame->Field(ID3FN_TEXT) = buff;
        AddFrame(trackFrame, true);
      }
    }
      
    // the GENRE field/frame
    if (temp.ucGenre != 255)
    {
      if (NULL == Find(ID3FID_CONTENTTYPE))
      {
        ID3_Frame *genreFrame;
          
        genreFrame = new ID3_Frame;
        if (NULL == genreFrame)
          ID3_THROW(ID3E_NoMemory);

        char buff[6];
        
        sprintf(buff, "(%lud)", (luint) temp.ucGenre);
        
        genreFrame->SetID(ID3FID_CONTENTTYPE);
        genreFrame->Field(ID3FN_TEXT) = buff;
        AddFrame(genreFrame, true);
      }
    }
  }
    
  return ;
}

// $Log: tag_parse_v1.cpp,v $
// Revision 1.5  1999/11/15 20:21:15  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
