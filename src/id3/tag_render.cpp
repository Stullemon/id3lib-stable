// $Id: tag_render.cpp,v 1.6 1999/11/19 19:27:27 scott Exp $

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

#include <string.h>
#include <memory.h>
#include <id3/tag.h>
#include <id3/misc_support.h>
#include <stdlib.h>

luint ID3_Tag::Render(uchar *buffer)
{
  luint bytesUsed = 0;
  
  if (NULL == buffer)
    ID3_THROW(ID3E_NoBuffer);

  ID3_Elem *cur = __pFrameList;
  ID3_TagHeader header;
    
  SetVersion(ID3_TAGVERSION, ID3_TAGREVISION);
    
  header.SetVersion(__ucVersion, __ucRevision);
  bytesUsed += header.Size();
    
  // set up the encryption and grouping IDs
    
  // ...
    
  while (cur)
  {
    if (cur->pFrame != NULL)
    {
      cur->pFrame->__bCompression = __bCompression;
      cur->pFrame->SetVersion(__ucVersion, __ucRevision);
      bytesUsed += cur->pFrame->Render(&buffer[bytesUsed]);
    }
      
    cur = cur->pNext;
  }
    
  if (__bSyncOn)
  {
    uchar *tempz;
    luint newTagSize;
      
    newTagSize = GetUnSyncSize(&buffer[header.Size()], 
                               bytesUsed - header.Size());
      
    if (newTagSize > 0 && (newTagSize + header.Size()) > bytesUsed)
    {
      tempz = new uchar[newTagSize];
      if (NULL == tempz)
        ID3_THROW(ID3E_NoMemory);

      UnSync(tempz, newTagSize, &buffer[header.Size()],
             bytesUsed - header.Size());
      header.SetFlags(ID3HF_UNSYNC);
          
      memcpy(&buffer[header.Size()], tempz, newTagSize);
      bytesUsed = newTagSize + header.Size();
      delete[] tempz;
    }
  }
    
  // zero the remainder of the buffer so that our padding bytes are zero
  for (luint i = 0; i < PaddingSize(bytesUsed); i++)
    buffer[bytesUsed + i] = 0;
      
  bytesUsed += PaddingSize(bytesUsed);
    
  header.SetDataSize(bytesUsed - header.Size());
  header.Render(buffer);
    
  // set the flag which says that the tag hasn't changed
  __bHasChanged = false;
  
  return bytesUsed;
}


luint ID3_Tag::Size(void) const
{
  luint bytesUsed = 0;
  ID3_Elem *cur = __pFrameList;
  ID3_TagHeader header;
  
  header.SetVersion(__ucVersion, __ucRevision);
  bytesUsed += header.Size();
  
  while (cur)
  {
    if (cur->pFrame)
    {
      cur->pFrame->SetVersion(__ucVersion, __ucRevision);
      bytesUsed += cur->pFrame->Size();
    }
    
    cur = cur->pNext;
  }
  
  // add 30% for sync
  if (__bSyncOn)
    bytesUsed += bytesUsed / 3;
    
  bytesUsed += PaddingSize(bytesUsed);
  
  return bytesUsed;
}


void ID3_Tag::RenderExtHeader(uchar *buffer)
{
  if (__ucVersion == 3 && __ucRevision == 0)
  {}
  
  return ;
}


void ID3_Tag::GenerateTempName(void)
{
  strcpy(__sTempName, __sFileName);
  strcat(__sTempName, "XXXXXX");
  if (mktemp(__sTempName) == NULL)
  {
    // Couldn't create a unique temporary name
    // Probably should throw an exception to that effect
  }
  
  return ;
}

void ID3_Tag::RenderV1ToHandle(void)
{
  ID3V1_Tag tagV1;
  char sTag[LEN_V1], *pCur = sTag, *sTemp = NULL;

  for (int i = 0; i < LEN_V1; i++)
    sTag[i] = '\0';

  strncpy(pCur, "TAG", LEN_V1_ID);
  pCur = &pCur[LEN_V1_ID];

  sTemp = ID3_GetTitle(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_TITLE);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_TITLE];

  sTemp = ID3_GetArtist(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_ARTIST);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_ARTIST];

  sTemp = ID3_GetAlbum(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_ALBUM);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_ALBUM];

  sTemp = ID3_GetYear(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_YEAR);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_YEAR];

  sTemp = ID3_GetComment(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_COMMENT);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1 - 2];

  sTemp = ID3_GetTrack(this);
  if (sTemp != NULL)
  {
    uchar ucTrack = (uchar) atoi(sTemp);
    pCur[0] = '\0';
    pCur[1] = ucTrack;
    delete [] sTemp;
  }

  char sID[LEN_V1_ID];

  // We want to check if there is already an id3v1 tag, so we can write over
  // it.  First, seek to the beginning of any possible id3v1 tag
  if (fseek(__fFileHandle, -LEN_V1, SEEK_END) != 0)
    // TODO:  This is a bad error message.  Make it more descriptive
    ID3_THROW(ID3E_NoData);

  // Read in the TAG characters
  if (fread(sID, 1, LEN_V1_ID, __fFileHandle) != LEN_V1_ID)
    // TODO:  This is a bad error message.  Make it more descriptive
    ID3_THROW(ID3E_NoData);

  // If those three characters are TAG, then there's a preexisting id3v1 tag,
  // so we should set the file cursor so we can overwrite it with a new tag.
  if (memcmp(sID, "TAG", LEN_V1_ID) == 0)
    if (fseek(__fFileHandle, -LEN_V1, SEEK_END) != 0)
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
  // Otherwise, set the cursor to the end of the file so we can append on 
  // the new tag.
  else
    if (fseek(__fFileHandle, 0, SEEK_END) != 0)
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);

  fwrite(sTag, 1, LEN_V1, __fFileHandle);
  __bHasV1Tag = true;
}

void ID3_Tag::RenderV2ToHandle(void)
{
  uchar *buffer;
  luint size = Size();
  
  if (NULL == __fFileHandle)
    ID3_THROW(ID3E_NoData);

  if (size > 0)
  {
    buffer = new uchar[size];
    if (NULL == buffer)
      ID3_THROW(ID3E_NoMemory);

    size = Render(buffer);      
    if (size > 0)
    {
      // if the new tag fits perfectly within the old and the old one
      // actually existed (ie this isn't the first tag this file has had)
      if ((__ulOldTagSize == 0 && __ulFileSize == 0) || 
          (size == __ulOldTagSize && size != 0))
      {
        fseek(__fFileHandle, 0, SEEK_SET);
        fwrite(buffer, 1, size, __fFileHandle);
        __ulOldTagSize = size;
      }
      else
      {
        // else we gotta make a temp file, copy the tag into it, copy the
        // rest of the old file after the tag, delete the old file, rename
        // this new file to the old file's name and update the __fFileHandle
        FILE *tempOut;
            
        if (strlen(__sTempName) > 0)
        {
          tempOut = fopen(__sTempName, "wb");
          if (NULL != tempOut)
          {
            uchar * buffer2;
                
            fwrite(buffer, 1, size, tempOut);
                
            fseek(__fFileHandle, __ulOldTagSize, SEEK_SET);
                
            buffer2 = new uchar [BUFF_SIZE];
            if (NULL == buffer2)
              ID3_THROW(ID3E_NoMemory);

            int i;
                  
            while (! feof(__fFileHandle))
            {
              i = fread(buffer2, 1, BUFF_SIZE, __fFileHandle);
              fwrite(buffer2, 1, i, tempOut);
            }
              
            delete[] buffer2;
                
            fclose(tempOut);
            fclose(__fFileHandle);
            remove(__sFileName);
            rename(__sTempName, __sFileName);
            OpenLinkedFile();
                
            __ulOldTagSize = size;
          }
        }
      }
    }
        
    delete[] buffer;
  }
    
  return ;
}


#define ID3_PADMULTIPLE (2048)
#define ID3_PADMAX  (4096)


luint ID3_Tag::PaddingSize(luint curSize) const
{
  luint newSize = 0;
  
  // if padding is switched off or there is no attached file
  if (! __bPadding || __ulFileSize == 0)
    return 0;
    
  // if the old tag was large enough to hold the new tag, then we will simply
  // pad out the difference - that way the new tag can be written without
  // shuffling the rest of the song file around
  if (__ulOldTagSize && (__ulOldTagSize > curSize) && 
      (curSize - __ulOldTagSize) < ID3_PADMAX)
    newSize = __ulOldTagSize;
  else
  {
    luint tempSize = curSize + __ulFileSize;
    
    // this method of automatic padding rounds the COMPLETE FILE up to the
    // nearest 2K.  If the file will already be an even multiple of 2K (with
    // the tag included) then we just add another 2K of padding
    tempSize = ((tempSize / ID3_PADMULTIPLE) + 1) * ID3_PADMULTIPLE;
    
    // the size of the new tag is the new filesize minus the song size
    newSize = tempSize - __ulFileSize;
  }
  
  return newSize - curSize;
}



// $Log: tag_render.cpp,v $
// Revision 1.6  1999/11/19 19:27:27  scott
// (RenderV1ToHandle): Added.  Adds or overwrites an id3v1 tag to a file
// handle.  Needs testing.
// (RenderV2ToHandle): Renamed from RenderToHandle.
//
// Revision 1.5  1999/11/15 20:21:29  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.  Now uses mktemp rather than mkstemp in
// GenerateTempName so that a file descriptor isn't created along
// with the temporary name.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
